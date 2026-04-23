#include <future>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

class thread_interrupted {};
void interruption_point();

class interrupt_flag {
	std::atomic<bool> flag;
	std::condition_variable* thread_cond;
	std::condition_variable_any* thread_cond_any;
	std::mutex set_clear_mtx;
public:
	interrupt_flag() : thread_cond(nullptr), thread_cond_any(nullptr) {}

	void set() {
		flag.store(true, std::memory_order_relaxed);
		std::lock_guard<std::mutex> lk(set_clear_mtx);
		if (thread_cond) {
			thread_cond->notify_all();
		} else if (thread_cond_any) {
			thread_cond_any->notify_all();
		}
	}

	template<typename Lockable>
	void wait(std::condition_variable_any& cv, Lockable& lk) {
		struct custom_lock {
			interrupt_flag* self;
			Lockable& lk;
			custom_lock(interrupt_flag* self_,
				Lockable& lk_,
				std::condition_variable_any& cond) : self(self_), lk(lk_) {
				self->set_clear_mtx.lock();
				self->thread_cond_any = &cond;
			}

			void unlock() {
				lk.unlock();
				self->set_clear_mtx.unlock();
			}

			void lock() {
				std::lock(self->set_clear_mtx, lk);
			}

			~custom_lock() {
				self->thread_cond_any = nullptr;
				self->set_clear_mtx.unlock();
			}
		};

		custom_lock cl(this, lk, cv);
		interruption_point();
		cv.wait(cl);
		interruption_point();
	}

	bool is_set() const {
		return flag.load(std::memory_order_relaxed);
	}

	void set_condition_variable(std::condition_variable& cv) {
		std::lock_guard<std::mutex> lk(set_clear_mtx);
		thread_cond = &cv;
	}

	void clear_condition_variable() {
		std::lock_guard<std::mutex> lk(set_clear_mtx);
		thread_cond = nullptr;
	}

	struct clear_cv_on_destruct {
		interrupt_flag* flag;
		clear_cv_on_destruct(interrupt_flag* f) : flag(f) {}
		~clear_cv_on_destruct() {
			flag->clear_condition_variable();
		}
	};
};

thread_local interrupt_flag this_thread_interrupt_flag;

void interruption_point() {
	if (this_thread_interrupt_flag.is_set()) {
		throw thread_interrupted();
	}
}

void interruptible_wait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk) {
	interruption_point();
	this_thread_interrupt_flag.set_condition_variable(cv);
	interrupt_flag::clear_cv_on_destruct guard(&this_thread_interrupt_flag);
	interruption_point();
	cv.wait_for(lk, std::chrono::milliseconds(1));
	interruption_point();
}

class interrupt_thread {
	std::thread internal_thread;
	interrupt_flag* flag;
public:
	template<typename FunctionType>
	interrupt_thread(FunctionType f) {
		std::promise<interrupt_flag*> p;
		internal_thread = std::thread([f,&p] {
			p.set_value(&this_thread_interrupt_flag);
			f();
		});
		flag = p.get_future().get();
	}
	void join();
	void detach();
	bool joinable() const;
	void interrupt() {
		if (flag) {
			flag->set();
		}
	}
};