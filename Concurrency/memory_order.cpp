#include <thread>
#include <memory>
#include <assert.h>
#include <print>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() {
	x.store(true, std::memory_order_seq_cst);
}

void write_y() {
	y.store(true, std::memory_order_seq_cst);
}

void write_x_before_y_seq() {
	while (!x.load(std::memory_order_seq_cst));
	if (y.load(std::memory_order_seq_cst)) {
		++z;
		std::println("X before Y --> x: {}, y: {}, z: {}", x.load(), y.load(), z.load());
	}
}

void write_y_before_x_seq() {
	while (!y.load(std::memory_order_seq_cst));
	if (x.load(std::memory_order_seq_cst)) {
		++z;
		std::println("Y before X --> x: {}, y: {}, z: {}", x.load(), y.load(), z.load());
	}
}

void write_x_before_y_relaxed() {
	x.store(true, std::memory_order_relaxed);
	y.store(true, std::memory_order_relaxed);
 {
		++z;
		std::println("X before Y --> x: {}, y: {}, z: {}", x.load(), y.load(), z.load());
	}
}

void write_y_before_x_relaxed() {
	while (!y.load(std::memory_order_relaxed));
	if (x.load(std::memory_order_relaxed)) {
		++z;
		std::println("Y before X --> x: {}, y: {}, z: {}", x.load(), y.load(), z.load());
	}
}

int main() {
	x = false;
	y = false;
	z = 0;

	std::thread	a(write_x);
	std::thread	b(write_y);
	std::thread	c(write_x_before_y_relaxed);
	std::thread	d(write_y_before_x_relaxed);

	a.join();
	b.join();
	c.join();
	d.join();

	assert(z.load() != 0);
}