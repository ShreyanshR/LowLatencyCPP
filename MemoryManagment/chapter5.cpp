#include <iostream>
#include <cassert>
template <class T> void print_pointee(T *p) {
	if (p) std::cout << *p << "\n";
}

template <class T>
std::unique_ptr<T> make_one(const T& arg) {
	return std::make_unique<T>(arg);
}

template <typename T>
bool point_to_same(const std::unique_ptr<T>& p0,
					const std::unique_ptr<T>& p1) {
	std::cout << "Size: " << sizeof(std::unique_ptr<T>) << std::endl;
	std::cout << "Size of T: " << sizeof(T*) << "\n";
	return p0 == p1;
}

template <typename T>
bool has_same_vale(const std::unique_ptr<T>& p0,
					const std::unique_ptr<T>& p1) {
	return p0 && p1 && *p0 == *p1;
}

#include <cstddef>

template <typename T>
class fixed_size_array {
	std::size_t nelems{};
	std::unique_ptr<T[]> elems{};
public:
	fixed_size_array() = default;
	auto size() const{return nelems;}
	bool empty() const {return size() == 0;}
	fixed_size_array(std::size_t n) : nelems(n), elems(new T[n]){}
	T& operator[](int n){return elems[n];}
	const T& operator[](int n) const {return elems[n];}
};

template <class T>
class pair_with_alloc {
	std::unique_ptr<T> p0, p1;
public:
	pair_with_alloc(const T& val0, const T& val1) : p0{std::make_unique<T>(val0)},
													p1{std::make_unique<T>(val1)} {

	}
	//destructor implicitly called
	//copy & move implicitly wokk
};

#include <random>
#include <string>
class risky {
	std::mt19937 prng{std::random_device{}()};
	std::uniform_int_distribution<int> penny{0,1};
public:
	risky() = default;
	risky(const risky&) {
		if (penny(prng)) throw 3;
	}
	~risky() {
		std::cout << "~risky()\n";
	}
};

int main() {
	if (std::string s0, s1; std::cin >> s0 >> s1)
	{
		try {
			pair_with_alloc a{s0, s1};
			pair_with_alloc b{risky{}, risky{}};
		} catch (...) {
			std::cerr << "Something was thrown...\n";
		}
	}

	auto p = make_one(3);
	print_pointee(p.get());

	std::unique_ptr<int> a {new int{3}};
	std::unique_ptr<int> b {new int{3}};

	assert(point_to_same(a,a) && has_same_vale(a,a));
	assert(!point_to_same(a,b) && has_same_vale(a,b));

}