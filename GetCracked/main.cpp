#include <iostream>
#include "vector.cpp"

class Dummy {
	int x, y, z;
public:
	Dummy(int x = 0, int y = 1) : x(x + 2), y(y + 3) {
		z = x + y;
	}
	void print() {
		std::cout << z << x << y;
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

	std::cout << 1/0 << std::endl;

	return 0;
}