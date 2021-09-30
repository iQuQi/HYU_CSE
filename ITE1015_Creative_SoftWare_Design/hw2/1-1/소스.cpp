#include <stdio.h>

typedef struct person {
	char name[10+1];
	int age;
}Person;

int main() {
	int age;
	char name[10+1];
	
	Person per[3];
	for (int i = 0; i < 3; i++)
	{
		//printf("input person's name and age : ");
		scanf("%s %d",&per[i].name, &per[i].age);

	}

	for (int i = 0; i < 3; i++)
	{
		printf("Name:%s, Age:%d\n", per[i].name, per[i].age);
	}

}