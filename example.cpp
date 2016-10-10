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

	unsigned int arr[] = {
		0x11111111,
		0x22222222,
		0x33333333,
		0x44444444,
		0x55555555,
		0x66666666,
		0x77777777,
		0x88888888,
		0x99999999,
		0xAAAAAAAA,
		0xBBBBBBBB,
		0xCCCCCCCC,
		0xDDDDDDDD,
		0xEEEEEEEE,
		0xFFFFFFFF
	};

	//test_stack_overflow();
	test_invalid_dereference();
	//test_invalid_write();

	return 0;
}
