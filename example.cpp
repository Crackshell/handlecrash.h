#include "handlecrash.h"

void test_stack_overflow()
{
	int a[10000];
	test_stack_overflow();
}

void test_invalid_dereference()
{
	int* a = (int*)0x12345678;
	printf("%d\n", *a);
}

void test_invalid_write()
{
	int* a = (int*)0x12345678;
	*a = 1;
}

int main()
{
	hc_install();

	//test_stack_overflow();
	test_invalid_dereference();
	//test_invalid_write();

	return 0;
}
