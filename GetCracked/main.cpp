#include <iostream>
#include "vector.cpp"
#include <thread>
#include <print>

class Dummy {
	int x, y, z;
public:
	Dummy(int x = 0, int y = 1) : x(x + 2), y(y + 3) {
		z = x + y;
	}
	void print() {
		std::cout << z << x << y << std::endl;
	}
};

int main() {
	getcracked::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	std::cout << v.get_size() <<"\n";

	Dummy d(7);
	d.print();

	//std::cout << 1/0 << std::endl;
	const int i = 0;
	int& r = const_cast<int&>(i);
	r = 1;

	std::cout << r << std::endl;

	int lhs{5};
	int rhs{10};

	std::thread t([&lhs,&rhs]() {
		int lhs_value = lhs;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		int rhs_value = rhs;
		std::println("T1: lhs_value: {}, rhs_value: {}", lhs_value, rhs_value);
		bool result = lhs_value==rhs_value;
		std::cout << "Result: " << result << std::endl;
		return lhs_value == rhs_value;
	});

	std::thread t1([&lhs, &rhs]() {
		lhs = 10;
		std::println("T2: lhs_value: {}, rhs_value: {}", lhs, rhs);

	});

	t.join();
	t1.join();

	return 0;
}