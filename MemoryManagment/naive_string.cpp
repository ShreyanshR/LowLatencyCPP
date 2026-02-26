#include <iostream>
#include <algorithm>

class naive_string {
private:
	char* p{};
	std::size_t nelems{};
	friend std::ostream& operator<<(std::ostream& os, const naive_string& other);

public:
	std::size_t size() const {
		return nelems;
	}

	bool empty() const {
		return size() == 0;
	}

	naive_string() = default;

	naive_string(const char *s) : nelems(std::strlen(s)) {
		//std::cout<<"Init " << "\n";
		//std::cout<<"Nelems: " << nelems << "\n";
		p = new char[size() + 1];
		std::copy_n(s,size(), p);
		p[size()] = '\0';
	}

	char operator[](std::size_t n) const { return p[n];}
	char& operator[](std::size_t n) {return p[n];}

	~naive_string() {
		delete [] p;
	}

	naive_string(const naive_string &other) : p(new char[other.size() + 1]), nelems(other.size()){
		std::copy_n(other.p, other.size(), p);
		p[size()] = '\0';
		std::cout << "Copy Constructor\n";
	}

	void swap(naive_string& other) noexcept {
		using std::swap;
		swap(p, other.p);
		swap(nelems, other.nelems);
	}

	naive_string& operator=(const naive_string& other) {
		naive_string{other}.swap(*this);
		std::cout << "Copy Assignment\n";
		return *this;
	}

	naive_string(naive_string&& other) noexcept : p(std::move(other.p)), nelems(std::move(other.nelems)) {
		other.p = nullptr;
		other.nelems = 0;
		std::cout << "Move Constructor\n";
	}

	naive_string& operator=(naive_string&& other) noexcept {
		naive_string{std::move(other)}.swap(*this);
		std::cout << "Move Assignment\n";
		return *this;
	}
	/*
	 Don't write code like this as it's a brute force approach
	naive_string& operator=(const naive_string& other) {
		if (this == &other) return *this;
		char* q = new char[other.size() + 1];
		delete [] p;
		p = q;
		std::copy(other.p, other.p + other.size(), p);
		nelems = other.size();
		p[size()] = '\0';
		return *this;
	}*/
};

std::ostream& operator<<(std::ostream& os,const naive_string& other) {
	os << "String: " << other.p << " , & no. of elems " << other.nelems <<"\n";
	return os;
}

bool force_alloc_fail = false;

void* operator new[](std::size_t size) {
	if (force_alloc_fail) {
		throw std::bad_alloc{};
	}
	void* p = std::malloc(size);
	if (!p) throw std::bad_alloc{};
	return p;
}

void operator delete[](void* p) noexcept {
	std::free(p);
}

int main() {
	naive_string s0{"What a find day"};
	naive_string s1 = s0;
	naive_string s2(s0);

	naive_string a{"hello"};
	naive_string b{"world"};

	std::cout << "Before assignment: " << a;

	force_alloc_fail = true;

	try {
		a = b;
	} catch (const std::bad_alloc& e) {
		std::cout<<"Allocation failed" <<"\n";
	}

	force_alloc_fail = false;

	std::cout << "a after failed assignment: " << a;
	std::cout << "b after failed assignment: " << b;



	return 0;
}