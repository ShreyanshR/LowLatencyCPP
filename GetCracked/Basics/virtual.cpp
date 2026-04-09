#include <iostream>
#include <array>
#include <string>
#include <string_view>

class Animal {
	protected:
	std::string m_name;

	Animal(std::string name) : m_name(name) {}

	Animal(const Animal&) = delete;
	Animal& operator=(const Animal&) = delete;

	public:
	const std::string& getName() const { return m_name; }
	virtual std::string_view speak() const {return "????";}
};

class Cat: public Animal {
public:
	Cat(std::string name) : Animal(name){}

	virtual std::string_view speak() const {return "Meow";}
};

class Dog: public Animal {
public:
	Dog(std::string name) : Animal(name){}
	virtual std::string_view speak() const {return "wow wow";}
};

void report(const Animal& animal) {
	std::cout << animal.getName() << " says " << animal.speak() << "\n";
}

#include <iostream>
#include <string_view>

class A
{
public:
	A() { std::cout << getName(); } // note addition of constructor (getName() now called from here)

	virtual std::string_view getName() const { return "A"; }
};

class B : public A
{
public:
	virtual std::string_view getName() const { return "B"; }
};

class C : public B
{
public:
	virtual std::string_view getName() const { return "C"; }
};

class D : public C
{
public:
	virtual std::string_view getName() const { return "D"; }
};

int main() {

	C c{};
	Cat cat{"Fred"};
	Dog dog{"bella"};

	report(cat);
	report(dog);

	return 0;
}