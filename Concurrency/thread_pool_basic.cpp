#include <atomic>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include "thread_safe_queue.h"

class join_threads {
	std::vector<std::thread>& threads;
public:
	explicit join_threads(std::vector<std::thread>& threads_) : threads(threads_){}
	~join_threads() {
		for (unsigned long i = 0; i < threads.size(); ++i) {
			if (threads[i].joinable()) {
				threads[i].join();
			}
		}
	}
};

class function_wrapper {
private:
	struct impl_base {
		virtual void call() = 0;
		virtual ~impl_base() = default;
 	};
	std::unique_ptr<impl_base> impl;
	template<typename F>
	struct impl_type : impl_base {
		F f;
		impl_type(F&& f_) : f(std::move
			(f_)){}
		void call(){f();}
	};
public:
	template <typename F>
	function_wrapper(F&& f) : impl(new impl_type<F>(std::move(f))) {}
	void operator()(){ impl->call();}
	function_wrapper() = default;
	function_wrapper(function_wrapper&& other) : impl(std::move(other.impl)) {}
	function_wrapper& operator=(function_wrapper&& other) {
		impl = std::move(other.impl);
		return *this;
	}
	function_wrapper(const function_wrapper& other) = delete;
	function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator=(const function_wrapper&) = delete;
	operator bool() const {return impl != nullptr;}
};

class thread_pool_basic {
private:
	std::atomic_bool done{false}; //when thread pool is done executing
	std::vector<std::thread> threads; //store all the threads
	join_threads joiner;
	thread_safe_queue<function_wrapper> work_queue;
	void work_thread() {
		while (!done) {
			function_wrapper task;
			if (work_queue.wait_and_pop(task) && task)
				task();
		}
	}

public:
	thread_pool_basic() : done(false), joiner(threads) {
		unsigned const thread_count = std::thread::hardware_concurrency();
		try {
			for (unsigned i = 0; i < thread_count; ++i) {
				threads.push_back(std::thread(&thread_pool_basic::work_thread, this));
				//so here when we call the thread pool we have to push the task
				//to the thread
			}
		}
		catch (...){
			done = true;
			throw;
		}
	}

	template<typename FunctionType>
	std::future<std::invoke_result_t<FunctionType>> submit(FunctionType f) {
		typedef typename std::invoke_result<FunctionType>::type result_type;
		std::packaged_task<result_type()> task(std::move(f));
		std::future<result_type> res(task.get_future());
		work_queue.push(std::move(task));
		return res;
	}
	~thread_pool_basic() {
		done = true;
		work_queue.shutdown_queue();
	}
/*
	void submit(std::function<void()> task) {
		work_queue.push(std::move(task));
	} */
};
// ── helpers for tests ──

int free_function_add(int a, int b) { return a + b; }

struct Functor {
	int multiplier;
	int operator()(int x) const { return x * multiplier; }
};

void run_tests() {
	int passed = 0, failed = 0;
	auto check = [&](bool cond, const char* name) {
		if (cond) { ++passed; std::cout << "  PASS: " << name << "\n"; }
		else      { ++failed; std::cout << "  FAIL: " << name << "\n"; }
	};

	// 1. void lambda — submit returns future<void>, should complete without error
	{
		thread_pool_basic pool;
		std::atomic<int> counter{0};
		auto fut = pool.submit([&counter]() { counter++; });
		fut.get();
		check(counter == 1, "void lambda");
	}

	// 2. lambda returning a value — future carries the result
	{
		thread_pool_basic pool;
		auto fut = pool.submit([]() -> int { return 42; });
		check(fut.get() == 42, "lambda returning int");
	}

	// 3. lambda returning std::string
	{
		thread_pool_basic pool;
		auto fut = pool.submit([]() -> std::string { return "hello"; });
		check(fut.get() == "hello", "lambda returning string");
	}

	// 4. free function (wrapped in a lambda to make it zero-arg)
	{
		thread_pool_basic pool;
		auto fut = pool.submit([]() { return free_function_add(3, 4); });
		check(fut.get() == 7, "free function via lambda");
	}

	// 5. functor / callable object
	{
		thread_pool_basic pool;
		Functor f{5};
		auto fut = pool.submit([f]() { return f(6); });
		check(fut.get() == 30, "functor");
	}

	// 6. multiple tasks — all futures resolve correctly
	{
		thread_pool_basic pool;
		std::vector<std::future<int>> futures;
		for (int i = 0; i < 100; ++i) {
			futures.push_back(pool.submit([i]() -> int { return i * i; }));
		}
		bool all_ok = true;
		for (int i = 0; i < 100; ++i) {
			if (futures[i].get() != i * i) { all_ok = false; break; }
		}
		check(all_ok, "100 tasks return correct values");
	}

	// 7. exception propagation through future
	{
		thread_pool_basic pool;
		auto fut = pool.submit([]() -> int { throw std::runtime_error("boom"); return 0; });
		bool caught = false;
		try { fut.get(); }
		catch (const std::runtime_error& e) { caught = (std::string(e.what()) == "boom"); }
		check(caught, "exception propagates through future");
	}

	// 8. tasks with shared state — atomic counter from many tasks
	{
		thread_pool_basic pool;
		std::atomic<int> sum{0};
		std::vector<std::future<void>> futures;
		for (int i = 1; i <= 50; ++i) {
			futures.push_back(pool.submit([&sum, i]() { sum += i; }));
		}
		for (auto& f : futures) f.get();
		check(sum == 1275, "50 tasks atomic accumulation (sum 1..50 = 1275)");
	}

	// 9. submit from multiple threads concurrently
	{
		thread_pool_basic pool;
		std::atomic<int> total{0};
		std::vector<std::thread> submitters;
		for (int t = 0; t < 4; ++t) {
			submitters.emplace_back([&pool, &total]() {
				for (int i = 0; i < 25; ++i) {
					auto fut = pool.submit([&total]() { total++; });
					fut.get();
				}
			});
		}
		for (auto& s : submitters) s.join();
		check(total == 100, "concurrent submit from 4 threads (25 each)");
	}

	std::cout << "\nResults: " << passed << " passed, " << failed << " failed\n";
}

int main() {
	run_tests();
	return 0;
}
