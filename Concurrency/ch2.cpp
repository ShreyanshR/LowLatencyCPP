#include <thread>
#include <iostream>

struct func {
	int& i;
	func(int& i_) : i(i_) {}

	void operator()() {
		for (uint32_t j = 0; j < 1000000; ++j) {
			i++;
		}
	}
};

class thread_guard {
	std::thread& t;
public:
	explicit thread_guard(std::thread& t_) : t(t_) {}
	~thread_guard() {
		if (t.joinable()) {
			t.join();
		}
	}
	thread_guard(thread_guard const&) = delete;
	thread_guard& operator=(thread_guard const&) = delete;
};

void oops() {
	int some_local_state = 0;
	func my_func(some_local_state);

	std::thread my_thread(my_func);
	my_thread.join();
	std::cout << some_local_state << std::endl;
}

class scoped_thread {
	std::thread t;

public:
	explicit scoped_thread(std::thread t_): t(std::move(t_)) {
		if (!t.joinable()) {
			throw std::logic_error("No Thread");
		}
	}

	~scoped_thread() {
		t.join();
	}

	scoped_thread(scoped_thread const&) = delete;
	scoped_thread& operator=(scoped_thread const&) = delete;

};

void f() {
	int some = 0;
	func my_func(some);
	std::thread t(my_func);
	thread_guard g(t);
	scoped_thread t1{std::thread(func(some))};
	some += 1;
	std::cout << some << std::endl;
}

class joining_thread {
	std::thread t;

public:
	joining_thread() noexcept=default;
	template<typename Callable, typename ...Args>
	explicit joining_thread(Callable&& func, Args&& ...args): t(std::forward<Callable>(func), std::forward<Args>(args)...)
	{}

	explicit joining_thread(std:: thread t_) : t(std::move(t_)){}
	joining_thread(joining_thread&& other) noexcept : t(std::move(other.t)){}
	joining_thread& operator=(joining_thread&& other) noexcept {
		if (joinable()) {
			join();
		}
		t=std::move(other.t);
		return *this;
	}
	joining_thread& operator=(std::thread other) noexcept {
		if (joinable()) {
			join();
		}
		t = std::move(other);
		return *this;
	}
	~joining_thread() noexcept {
		if (joinable())
			join();
	}
	void swap(joining_thread& other) noexcept {
		t.swap(other.t);
	}
	std::thread::id get_id() const noexcept {
		return t.get_id();
	}
	bool joinable() const noexcept {
		return t.joinable();
	}
	void join() {
		t.join();
	}
	void detach() {
		t.detach();
	}
	std::thread& as_thread() noexcept {
		return t;
	}
	const std::thread& as_thread() const noexcept {
		return t;
	}
};

int do_someting(int val) {
	for (int i = 0; i < 100000; i++) {
		val++;
	}
	return val;
}
void function() {
	int local_state = 0;
	std::thread t([&](){
	  for(int i = 0; i < 1000000; i++) do_someting(local_state);
	});
	t.detach();
}

int main() {
	//oops();
	f();
	function();
}