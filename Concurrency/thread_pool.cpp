#include <atomic>
#include <functional>
#include <future>

#include "thread_safe_queue.h"
#include <vector>
#include <thread>

class join_threads {
	std::vector<std::thread>& threads;
public:
	explicit join_threads(std::vector<std::thread>& threads_) : threads(threads_) {}
	~join_threads() {
		for (auto& t : threads) {
			if (t.joinable()) t.join();
		}
	}
};

class function_wrapper {
	struct impl_base {
		virtual void call()=0;
		virtual ~impl_base(){};
	};
	std::unique_ptr<impl_base> impl;
	template<typename F>
	struct impl_type: impl_base {
		F f;
		impl_type(F&& f_) : f(std::move(f_)){}
		void call() {f();}
	};
public:
	template<typename F>
	function_wrapper(F&& f): impl(new impl_type<F>(std::move(f))) {}
	void operator()(){impl->call();}
	function_wrapper() = default;
	function_wrapper(function_wrapper&& other): impl(std::move(other.impl)) {}
	function_wrapper& operator=(function_wrapper&& other) {
		impl= std::move(other.impl);
		return *this;
	}
	function_wrapper(const function_wrapper&) = delete;
	function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator=(const function_wrapper&) = delete;
};

class thread_pool {
	std::atomic_bool done;
	thread_safe_queue<function_wrapper> work_queue;
	std::vector<std::thread> threads;
	join_threads joiner;
	void worker_thread() {
		while (!done) {
			function_wrapper task;
			if (work_queue.try_pop(task)) {
				task();
			}
			else {
				std::this_thread::yield();
			}
		}
	}

public:
	thread_pool(): done(false), joiner(threads) {
		unsigned const thread_count=std::thread::hardware_concurrency();
		try {
			for (unsigned i = 0; i < thread_count; ++i) {
				threads.push_back(std::thread(&thread_pool::worker_thread, this));
			}
		}
		catch (...) {
			done = true;
			throw;
		}
	}
  ~thread_pool() {
		done = true;
	}
	template<typename FunctionType>
	std::future<std::invoke_result_t<FunctionType>> submit(FunctionType f) {
		typedef std::invoke_result_t<FunctionType> result_type;
		std::packaged_task<result_type()> task(std::move(f));
		std::future<result_type> res(task.get_future());
		work_queue.push(std::move(task));
		return res;
	}
	/*
	void submit(FunctionType f) {
		work_queue.push(std::function<void()>(f));
	}
	*/
};
