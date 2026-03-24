#include <exception>
#include <memory>
#include <mutex>
#include <stack>
#include <print>
#include <thread>
#include <condition_variable>

struct empty_stack: std::exception {
	const char* what() const throw() { return "empty stack"; }
};

struct Explosive {
	int id;
	Explosive(int id) : id(id){}
	Explosive(const Explosive& other) {
		//id = other.id;
		throw std::runtime_error("Cant copy");
	}
	Explosive& operator=(const Explosive) = delete;
	Explosive(Explosive&& other) noexcept{
		this->id = other.id;
		other.id = 0;

	}
};

template<typename T>
class threadsafe_stack {
private:
	std::stack<T> data;
	mutable std::mutex m;
	std::condition_variable cv;
	//bool ready = false;

public:
	threadsafe_stack(){}
	threadsafe_stack(const threadsafe_stack& other) {
		std::lock_guard<std::mutex> lock(other.m);
		data = other.data;
	}
	threadsafe_stack& operator=(const threadsafe_stack&) = delete; // delete copy assignment
	void push(T new_value) {
		std::lock_guard<std::mutex> lock(m);
		data.push(std::move(new_value));
		//std::println("Pushed: {}", data.top());
		cv.notify_one();
	}
	std::shared_ptr<T> pop() {
		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock, [this]{return !data.empty();});
		//if (data.empty()) throw empty_stack();
		std::shared_ptr<T> const res(
			std::make_shared<T>(std::move(data.top())));
		data.pop();
		//std::println("Popped: {}", *res);
		return res;
	}
	void pop(T& value) {
		std::lock_guard<std::mutex> lock(m);
		if (data.empty()) throw empty_stack();
		value = std::move(data.top());
		data.pop();
	}
	bool empty() const {
		std::lock_guard<std::mutex> lock(m);
		return data.empty();
	}
};

int main() {
	threadsafe_stack<int> ts;
	threadsafe_stack<Explosive> es;


	std::thread t1([&]() {
		for (int i=0; i < 10; i++) {
			es.push(Explosive{i});
			std::println("Pushed id: {}", i);
		}
	});

	std::thread t2([&]() {
		for (int i=0; i < 10; i++) {
			auto val = es.pop();
			std::println("Popped id: {}", val->id);
		}
	});

	t1.join();
	t2.join();
}