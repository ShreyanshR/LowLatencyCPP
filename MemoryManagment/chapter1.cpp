#include <iostream>
#include <string>
#include <format>

struct X {
	std::string s;
	X(std::string_view s) : s(s) {
		std::cout<<std::format("X::X({})\n", s);
	}
	~X() {
		std::cout << std::format("X::X() for {}\n",s);
	};
};

X glob{"glob"};
void g() {
	X xg{"g()"};
}

struct Tiny {};

class Z {};
class Y {
	Z z;
};

struct Point2D{
	float x{}, y{};
	Point2D(float x, float y) : x(x), y(y){}
	Point2D() = default;
};

int main() {
	Point2D pt;
	std::cout << sizeof(Tiny) << std::endl;
	static_assert(sizeof(Z) > 0);
	static_assert(sizeof(Y) == sizeof(Z));

	X *p0 = new X{"p0"};
	[[maybe_unused]] X *p1 = new X{"p1"};
	X xmain{"main()"};
	g();
	delete p0;
}