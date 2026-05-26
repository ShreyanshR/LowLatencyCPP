#include "thread_safe_queue.h"
#include <iostream>
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