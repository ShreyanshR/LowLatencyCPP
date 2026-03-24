#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <print>

struct Name {
	std::string name;
	Name(std::string name) : name(name){}
	Name(const Name& other) {
		name = other.name;
	}
	Name& operator=(const Name&) = delete;
	Name(const Name&&) = delete;
	Name&& operator=(const Name&&) = delete;
};

template<typename T>
class threadsafe_queue {
private:
	mutable std::mutex mut;
	std::queue<T> data_queue;
	std::condition_variable data_cond;

public:
	threadsafe_queue(){}

	//threadsafe_queue& operator[]
	threadsafe_queue(const threadsafe_queue& other) {
		std::lock_guard<std::mutex> lock(other.mut);
		data_queue = other.data_queue;
	}
	threadsafe_queue& operator=(const threadsafe_queue&) = delete;
	void push(T new_value) {
		std::lock_guard<std::mutex> lock(mut);
		data_queue.push(new_value);
		data_cond.notify_one(); //will notify pop
	}
	bool try_pop(T& value) {
		std::lock_guard<std::mutex> lock(mut);
		std::println("Before empty");
		if (data_queue.empty()) {
			return false;
		}
		value = data_queue.front();
		std::println("Popped: {}", value);
		data_queue.pop();
		return true;
	}
	std::shared_ptr<T> try_pop() {
		std::lock_guard<std::mutex> lock(mut);
		if (data_queue.empty()) {
			return std::shared_ptr<T>();
		}
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));

		data_queue.pop();
		return res;
	}
	void wait_and_pop(T& value) {
		std::unique_lock<std::mutex> lock(mut);
		data_cond.wait(lock, [this]{return !data_queue.empty();});
		value = data_queue.front();
		data_queue.pop();
	}
	std::shared_ptr<T> wait_and_pop() {
		std::unique_lock<std::mutex> lock(mut);
		data_cond.wait(lock, [this]{return !data_queue.empty();});
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
		data_queue.pop();
		return res;
	}
	bool empty() const {
		std::lock_guard<std::mutex> lock(mut);
		return data_queue.empty();
	}
};

int main() {
	threadsafe_queue<std::string> ts;
	threadsafe_queue<Name> ns;

	std::thread t1([&]() {
		std::string s{'a'};
		for (int i = 0; i < 10; i++) {
			std::string val = s + "_" +  std::to_string(i);
			ns.push(val);
			std::println("Pushed: {}", val);
		}
	});

	std::thread t2([&]() {
		for (int i = 0; i < 10; i++) {
			auto val = ns.try_pop();
			if (val) {
				std::println("Popped: {}", val->name);
			} else {
				std::println("Empty queue");
			}


			//std::println("Pushed: {}", val);

		}
	});

	t1.join();
	t2.join();
}
