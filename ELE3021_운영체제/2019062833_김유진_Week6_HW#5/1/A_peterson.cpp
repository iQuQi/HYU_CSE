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


//락을 걸어주는 함수
void lock(int self) {
	
	//self에 해당하는 flag를 1로 변경하고 턴을 상대에게 넘긴다
	//이후 상대가 critical section에 있는 동안 while문을 돌며 기다린다

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

//락을 해제하는 함수
void unlock(int self) {
	//self에 값에 해당하는 flag를 0으로 변경
	if (self == 0) {
		flag0.store(0,MOSC);
	}
	else {
		flag1.store(0,MOSC);
	}
}


//스레드가 하는 일 - 공유변수증가
void* func(void *s) {
	int* thread_num = (int *)s;
	int i;
	for (i = 0; i < COUNT_NUM; i++)
	{
		//공유변수를 수정하기 전후로 locking 방식이 적용
		lock(*thread_num);
		critical_section_variable++;
		unlock(*thread_num);
	}
}


int main() {

	pthread_t p1, p2;
	int parameter[2] = { 0,1 };

	//스레드를 생성
	pthread_create(&p1, NULL, func, (void *)&parameter[0]);
	pthread_create(&p2, NULL, func, (void *)&parameter[1]);

	//스레드가 일을 끝낼 때 까지 기다림
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);

	//변경 후 공유변수의 실제값과 예상값을 각각 출력한다.
	printf("Actual Count: %d \\ Expected Count: %d\n",critical_section_variable,COUNT_NUM*2);
	return 0;
}
