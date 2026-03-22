#include <thread>
#include <iostream>
#include <mutex>
#include <optional>
#include <vector>
#include <print>

template <typename T, size_t Size>
class RingBuffer {
private:
	std::vector<T> buffer;
	size_t head = 0;
	size_t tail = 0;
	bool full = false;
	std::mutex mtx;

public:
	RingBuffer() : buffer(Size){}

	bool empty() {
		//std::lock_guard<std::mutex> lock(mtx);
		return (!full && head == tail);
	}

	int size() {
		if (full) return Size;
		if (head >= tail) return head - tail;
		return Size + head - tail;
	}

	bool push(const T &item) {
		std::lock_guard<std::mutex> lock(mtx);
		if (full)
			return false;

		buffer[head] = item;
		head = (head + 1) % Size;
		full = (head == tail);
		return true;
	}

	std::optional<T> pop() {
		std::lock_guard<std::mutex> lock(mtx);
		if (empty()) { //if it's empty return nullopt
			return std::nullopt;
		}

		auto val = buffer[tail];
		full = false;
		tail = (tail + 1) % Size;
		return val;
	}

	void print_elements() {
		std::println("Head: {}, Tail: {}", head, tail);
		for (auto& num: buffer) {
			std::cout << num << "\n";
		}
	}
};

int main() {
	RingBuffer<int, 5> rb;

	for (int i = 0; i < 10; i++) {
		rb.push(i);
	}
	rb.pop();
	rb.print_elements();
	rb.pop();
	rb.print_elements();



	/*
	std::thread producer([&]() {
		for (int i = 0; i < 1; i++) {
			while (!rb.push(i)) {
				std::this_thread::yield();
			}
			std::cout << "Produced: " << i;
		}
	});


	std::thread consumer([&]() {
		for (int i = 0; i < 1; i++) {
			std::optional<int> val;
			while (!(val = rb.pop())) {
				std::this_thread::yield();
			}
			std::cout << " Consumed: " << *val << " ";
		}
	});

	producer.join();
	consumer.join();
	*/
}