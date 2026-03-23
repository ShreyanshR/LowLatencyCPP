//
// Created by Shreyansh Rathore on 09/03/2026.
//

#include <list>
#include <mutex>
#include <algorithm>
#include <vector>
#include <thread>
#include <iostream>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int newValue) {
	std::lock_guard<std::mutex> guard(some_mutex);
	some_list.push_back(newValue);
}

bool list_contains(int valuToFind) {
	std::lock_guard<std::mutex> guard(some_mutex);
	return std::find(some_list.begin(), some_list.end(), valuToFind) != some_list.end();
}

class SafeList {
	std::mutex mtx;
	std::vector<int> data;
public:
	std::vector<int>& get_data() {
		std::cout << "[thread " << std::this_thread::get_id() << "] mutex locking\n";
		std::lock_guard<std::mutex> lk(mtx);
		std::cout << "[thread " << std::this_thread::get_id() << "] mutex LOCKED\n";
		return data;
	}

	void add(int val) {
		std::lock_guard<std::mutex> kl(mtx);
		data.push_back(val);
	}

	template<typename Func>
	void for_each(Func f) {
		std::lock_guard<std::mutex> lk2(mtx);
		for (auto& num: data) {
			f(num);
		}
	}
};


int main() {
	SafeList sf;
	sf.add(10);
	sf.add(20);
	sf.add(30);

	sf.for_each([&sf](int& x) {
		sf.add(99); // calling add() INSIDE for_each's lambda
	});

	sf.for_each([](int& x){ x = x*2;});

	sf.for_each([](int x ) {
		std::cout << x << "\n";
	});

	for (int i = 0; i < 10; i++) {
		sf.add(i);
	}

	sf.for_each([](int x ) {
		std::cout << x << "\n";
	});


	//auto& ref = sf.get_data(); // grab reference

	//std::cout << "--- mutex is NOW unlocked ---\n";
	//std::cout << "--- but I still have the reference ---\n";

	//ref.push_back(99); // no mutex. nothing protecting this.
	//std::cout << "pushed 99 with zero locking\n";
	//std::cout << "WTF1" << "\n";
	/*std::thread t1([&]() {
		for (int i = 0; i < 1000; i++) {
			sf.get_data().push_back(i);
		}
	});

	std::thread t2([&]() {
		for (int i=0; i < 1000; i++) {
			sf.get_data().push_back(i);
		}
	});

	t1.join();
	t2.join();
	std::cout << "WTF" << "\n";

	std::cout << sf.get_data().size() << std::endl;
	std::cout << "WTF" << "\n";
	*/
}