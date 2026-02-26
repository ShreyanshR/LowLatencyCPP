#include <iostream>

void f(int x) {
	std::cout << x << "\n";
}

int main() {
	int vals[]{2,3,5,7,11};
	enum{N = sizeof vals / sizeof vals[0]};

	for (const int val : vals)
		f(val);

	for (int *p = vals; p != vals + N; ++p) {
		f(*p);
	}

	int arr[10]{};
	int *p = &arr[3];
	p += 4;
	--p;

	p +=4;
	std::cout << *p << "\n";
	++p;
}