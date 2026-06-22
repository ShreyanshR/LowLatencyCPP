#include <cstdio>

struct Incomplete;                       // fine: pointers don't need the definition

int main() {
	printf("void*               : %zu\n", sizeof(void*));
	printf("int*                : %zu\n", sizeof(int*));
	printf("double*             : %zu\n", sizeof(double*));
	printf("Incomplete*         : %zu\n", sizeof(Incomplete*));
	printf("function ptr        : %zu\n", sizeof(void(*)()));

	struct Plain { int x; void f(); };
	printf("ptr-to-data-member  : %zu\n", sizeof(&Plain::x));   // 8
	printf("ptr-to-member-func  : %zu\n", sizeof(&Plain::f));   // often 16
}