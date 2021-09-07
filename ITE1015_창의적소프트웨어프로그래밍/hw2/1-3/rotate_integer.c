#include <stdio.h>

void rotateLeft(int *pa, int *pb, int* pc);
void rotateRight(int *pa, int *pb, int* pc);

int main() {
	int a=10, b=20, c=30;
	int menu;
	printf("%d:%d:%d\n", a, b, c);

	while (1) {
		scanf("%d", &menu);
		if (menu == 1) {
			rotateLeft(&a,&b,&c);
			printf("%d:%d:%d\n", a, b, c);

		}
		else if (menu == 2) {
			rotateRight(&a, &b, &c);
			printf("%d:%d:%d\n", a, b, c);

		}
		else break;
	}



}

void rotateLeft(int * pa, int * pb, int * pc)
{
	int temp;
	temp= *pa;
	*pa = *pb;
	*pb = *pc;
	*pc = temp;
}

void rotateRight(int * pa, int * pb, int * pc)
{
	int temp;
	temp = *pc;
	*pc = *pb;
	*pb = *pa;
	*pa = temp;
	
}
