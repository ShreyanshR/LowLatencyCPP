#include <future>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

int work(int id) {
	std::chrono::milliseconds(200);
	std::cout << id << "\n";
	return id;
}

int main() {
	std::vector<std::future<int>> op;
	for (int i = 0; i < 3; ++i) {
		 op.push_back(std::async(std::launch::deferred, work, i));
	}

	for (auto& f: op) {
		f.get();
	}

}
