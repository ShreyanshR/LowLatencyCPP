#include <atomic>
#include <functional>
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

class thread_pool_basic {
private:
	std::atomic_bool done{false}; //when thread pool is done executing
	std::vector<std::thread> threads; //store all the threads
	join_threads joiner;
	thread_safe_queue<std::function<void()>> work_queue;
	void work_thread() {
		while (!done) {
			std::function<void()> task;
			work_queue.wait_and_pop(task);
			if (task) task();
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
	~thread_pool_basic() {
		done = true;
		work_queue.shutdown_queue();
	}

	void submit(std::function<void()> task) {
		work_queue.push(std::move(task));
	}
};

int main() {
	thread_pool_basic pool;
	for (int i = 0; i < 5; ++i) {
		pool.submit([i]() {
			std::cout << "Task " << i << " running on thread " << std::this_thread::get_id() << "\n";
		});
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "Main: all tasks submitted, pool going out of scope...\n";
	std::cout << "finised" << "\n";
	return 0;
}
