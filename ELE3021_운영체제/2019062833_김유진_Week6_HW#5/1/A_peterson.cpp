#include <pthread.h>
#include <stdio.h>
#include <atomic>
#include <stdlib.h>
#include <iostream>


#define COUNT_NUM 2000000
#define MOSC std::memory_order_seq_cst
int critical_section_variable = 0;
std::atomic_int turn(0);
std::atomic_int flag0(0),flag1(0);


//���� �ɾ��ִ� �Լ�
void lock(int self) {
	
	//self�� �ش��ϴ� flag�� 1�� �����ϰ� ���� ��뿡�� �ѱ��
	//���� ��밡 critical section�� �ִ� ���� while���� ���� ��ٸ���

	if (self == 0) {
		flag0.store(1,MOSC);
		turn.store(1 - self,MOSC);
		while (flag1.load(MOSC)&&turn.load(MOSC)==1 - self);
	}
	else {
		flag1.store(1,MOSC);
		turn.store(1 - self,MOSC);
		while (flag0.load(MOSC)&&turn.load(MOSC)==1 - self);
	}
	
	
}

//���� �����ϴ� �Լ�
void unlock(int self) {
	//self�� ���� �ش��ϴ� flag�� 0���� ����
	if (self == 0) {
		flag0.store(0,MOSC);
	}
	else {
		flag1.store(0,MOSC);
	}
}


//�����尡 �ϴ� �� - ������������
void* func(void *s) {
	int* thread_num = (int *)s;
	int i;
	for (i = 0; i < COUNT_NUM; i++)
	{
		//���������� �����ϱ� ���ķ� locking ����� ����
		lock(*thread_num);
		critical_section_variable++;
		unlock(*thread_num);
	}
}


int main() {

	pthread_t p1, p2;
	int parameter[2] = { 0,1 };

	//�����带 ����
	pthread_create(&p1, NULL, func, (void *)&parameter[0]);
	pthread_create(&p2, NULL, func, (void *)&parameter[1]);

	//�����尡 ���� ���� �� ���� ��ٸ�
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);

	//���� �� ���������� �������� ������ ���� ����Ѵ�.
	printf("Actual Count: %d \\ Expected Count: %d\n",critical_section_variable,COUNT_NUM*2);
	return 0;
}
