#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <memory>
#include <thread>
#include <print>

struct Name {
	std::string name;
	Name(std::string name) : name(name){}
	Name(const Name& other) {
		name = other.name;
	}
	Name& operator=(const Name& other) = delete;
	Name(const Name&& other) = delete;
	Name&& operator=(const Name&& other) = delete;
};

template<typename T>
class thread_safe_queue {
private:
	mutable std::mutex mtx;
	std::queue<T> data_queue;
	std::condition_variable data_cond;

public:
	thread_safe_queue(){}
	thread_safe_queue(const thread_safe_queue& other) {
		std::lock_guard<std::mutex> lk(other.mtx);
		data_queue = other.data_queue;
	}
	thread_safe_queue& operator=(const thread_safe_queue& other) = delete;

	void push(T new_value) {
		std::lock_guard<std::mutex> lk(mtx);
		data_queue.push(new_value);
		data_cond.notify_one();
	}

	bool try_pop(T& value) {
		std::lock_guard<std::mutex> lk(mtx);
		if (data_queue.empty()) return false;
		value = data_queue.front();
		data_queue.pop();
		return true;
	}
	std::shared_ptr<T> try_pop() {
		std::lock_guard<std::mutex> lk(mtx);
		if (data_queue.empty()) return std::shared_ptr<T>();
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
		data_queue.pop();
		return res;
	}

	void wait_and_pop(T& value) {
		std::unique_lock<std::mutex> lk(mtx);
		data_cond.wait(lk, [this]{return !data_queue.empty();});
		value = data_queue.front();
		data_queue.pop();
	}

	std::shared_ptr<T> wait_and_pop() {
		std::unique_lock<std::mutex> lk(mtx);
		data_cond.wait(lk, [this]{return !data_queue.empty();});
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
		data_queue.pop();
		return res;
	}

	bool empty() const {
		std::lock_guard<std::mutex> lk(mtx);
		return data_queue.empty();
	}
};

int main() {
	thread_safe_queue<Name> ts;

	std::thread t1([&]() {
		std::string s{'a'};
		for (int i = 0; i < 10; i++) {
			std::string val = s + "_" + std::to_string(i);
			ts.push(val);
			std::println("Pushed-->{}", val);
		}
	});

	std::thread t2([&]() {
		for (int i = 0; i < 5; i++) {
			auto val = ts.wait_and_pop();
			if (val) {
				std::println("Popped from t2-->{}", val->name);
			} else {
				std::println("Queue is empty.");
			}
		}
	});

	std::thread t3([&]() {
		for (int i = 0; i < 5; i++) {
			auto val = ts.wait_and_pop();
			if (val) {
				std::println("Popped from t3-->{}", val->name);
			} else {
				std::println("Queue is empty.");
			}
		}
	});

	t1.join();
	t2.join();
	t3.join();
}