#include <semaphore>
#include <iostream>
#include <thread>

std::counting_semaphore<1> signal2Pong(0);
std::counting_semaphore<1> signal2Ping(0);

std::atomic<int> counter{};
constexpr int count_limit = 1'000'000;

void ping() {
	while (counter <= count_limit) {
		signal2Ping.acquire();
		counter++;
		signal2Pong.release();
	}
}

void pong() {
	while (counter < count_limit) {
		signal2Pong.acquire();
		counter++;
		signal2Ping.release();
	}
}

int main() {
	auto start = std::chrono::system_clock::now();
	signal2Ping.release();

	std::thread t1(ping);
	std::thread t2(pong);

	t1.join();
	t2.join();


	std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;

	std::cout << "Time: " << dur.count() << "\n";

	return 0;
}