#include <thread>
#include <atomic>
#include <print>

std::atomic<int> x(0), y(0), z(0);
std::atomic<bool> go(false);
unsigned const loop_count = 10;

struct read_values {
	int x, y, z;
};

read_values v1[loop_count];
read_values v2[loop_count];
read_values v3[loop_count];
read_values v4[loop_count];
read_values v5[loop_count];

void increment(std::atomic<int>* var_to_inc, read_values* values) {
	while (!go) std::this_thread::yield();
		// spin lock, it means it will stop the excecution until go = true
	for (unsigned i = 0; i < loop_count; ++i) {
		values[i].x = x.load(std::memory_order_relaxed);
		values[i].y = y.load(std::memory_order_relaxed);
		values[i].z= z.load(std::memory_order_relaxed);
		var_to_inc->store(i+1, std::memory_order_relaxed);
		std::this_thread::yield();
	}
}

void read_vals(read_values* values) {
	while (!go) std::this_thread::yield();
	for (unsigned i = 0; i < loop_count; ++i) {
		values[i].x = x.load(std::memory_order_relaxed);
		values[i].y = y.load(std::memory_order_relaxed);
		values[i].z = z.load(std::memory_order_relaxed);
	}
}

void print_values(read_values* v) {
	for (unsigned i = 0; i < loop_count; i++) {
		if (i)
			std::println(",");
		std::println("x: {}, y: {}, z: {}", v[i].x , v[i].y , v[i].z);
	}
}

int main() {
	std::thread t1(increment, &x, v1 );
	std::thread t2(increment, &y, v2);
	std::thread t3(increment, &z, v3);
	std::thread t4(read_vals, v4);
	std::thread t5(read_vals, v5);
	go = true;
	t5.join();
	t4.join();
	t3.join();
	t2.join();
	t1.join();

	print_values(v1);
	print_values(v2);
	print_values(v3);
	print_values(v4);
	print_values(v5);

}