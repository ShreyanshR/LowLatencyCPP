#include <iostream>
#include <vector>
#include <cstdio>

FILE file_;

FILE* open_file(const char *) {
	std::cout << "Call to open\n";
	return &file_;
}

void close_file(FILE*) {
	std::cout <<"Call to close\n";
}

class FileCloser {
	FILE* file;
public:
	FileCloser(FILE* file) : file(file){}
	~FileCloser() {
		close_file(file);
	}
};

int read_from(FILE*, char*, int) {
	return 0;
}

void process(const std::vector<char>&){}

enum{N=1024};

void f(const char* name) {
	FILE *file = open_file((name));
	if (!file) return;
	FileCloser fc{file}; //fc manages the file now
	std::vector<char> v;
	char buf[N];

	for (int n = read_from(file, buf, N); n!=0; n=read_from(file, buf, N))
		v.insert(end(v), buf + 0, buf + n);

	process(v);
}//close_file(file) through sf's destructor

template <class F> class scoped_finalizer {
	F f;
public:
	scoped_finalizer(const scoped_finalizer&) = delete; //deleting copy construcotr
	scoped_finalizer& operator=(const scoped_finalizer&) = delete; //deleting copy assignment
	scoped_finalizer(F f) : f{f}{}
	~scoped_finalizer() {
		f();
	}
};

class Darn{};
void k(){ throw 3;}

struct Evil {
	Evil(){std::cout << "Evil::Evil()\n";}
	~Evil() noexcept(false) {
		std::cout << "Evil::~Evil()\n";
		throw Darn{};
	}
};

void g() {
	std::cout << "A\n";
	Evil e;
	std::cout << "B\n";
	k();
	std::cout << "C\n";
}

#include <format>

struct Verbose {
	int n;
	Verbose(int n) : n{n} {
		std::cout << std::format("Verbose({})\n", n);
	}
	~Verbose() {
		std::cout << std::format("~Verbose({})\n", n);
	}
};

class X {
	static inline Verbose v0{0}; //global object
	Verbose v1{1};
};

Verbose v2{2};
static void l() {
	static Verbose v3{3}; //global object
	Verbose v4{4};
}

static void n() {
	static Verbose v5{5};
}



int main() {
	Verbose v6{6};
	{
		Verbose v7{7};
		l();
		X x;
	}
	l();
	X x;

}
	//f("test.txt");
	/*try {
		g();
	} catch (int) {
		std::cerr << "catch(int)\n";
	} catch (Darn) {
		std::cerr << "darn...\n";
	}*/
