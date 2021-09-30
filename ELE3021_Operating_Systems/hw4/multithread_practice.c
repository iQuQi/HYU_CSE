#include <stdio.h>
#include <stdlib.h>

#define ARGUMENT_NUMBER 20
long long result = 0;

void * ThreadFunc(void* n) {
	long long i;
	long long number = *((long long *)n);
	printf("number = %lld\n", number);
	for (i = 0; i < 25000000; i++)
	{
		result += number;
	}
}
int main() {
	long long argument[ARGUMENT_NUMBER];
	long long i;

	for ( i = 0; i < ARGUMENT_NUMBER; i++)
	{
		argument[i]=i;
	}

	for (i = 0; i < ARGUMENT_NUMBER; i++)
	{
		ThreadFunc((void*)&argument[i]);
	}

	printf("result = %lld\n", result);
	return 0;

}
