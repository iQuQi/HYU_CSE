#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define COUNT_NUM 2000000

//공유메모리에 쓸 구조체
struct smStruct{
	int processidassign;
	int turn;
	int flag[2];
	int critical_section_variable;
};

//lock을 걸어주는 함수
void lock(struct smStruct * sm,int myorder) {
	    //자신의 flag를 1로 변경하고 상대에게 턴을 넘긴다
		sm->flag[myorder]=1;
		sm->turn=1-myorder;
		//상대가 critical 존에 있는 동안 기다린다.
		while (sm->flag[1-myorder]&&sm->turn==1 - myorder);
}

//lock을 해제하는 함수
void unlock(struct smStruct * sm,int myorder) {
	//자신의 flag를 0으로 설정
	sm->flag[myorder]=0;
	
}


int main(){
	//자신의 pid를 받아오기
	pid_t pid = getpid();

	int my_order=0;
	int shmid;
	void * shmaddr;
	int ret;

	
	

	//공유 메모리를 생성/이미 있다면 가져오기
	int local_cnt=0;
	shmid=shmget((key_t)1234,sizeof(struct smStruct),IPC_CREAT|0666);
	if(shmid == -1) return 0;

	//공유메모리를 자신의 주소 공간에 붙임
	shmaddr=shmat(shmid,(void*)0,0);
	if(shmaddr==(char*)-1)return 0;



	//sm 변수안에 공유메모리의 영역을 가져와둔다.
	struct smStruct * sm;
	sm=(struct smStruct *)shmaddr;


	//공유메모리에 저장되어있는 값을 통해 각 프로세스에 다른 order을 부여한다
	if (pid == sm->processidassign)my_order = 0;
	else my_order = 1;

	// 자신의 flag 배열에서의 인덱스를 출력
	printf("my order is %d, pid is %d\n", my_order, pid);

	//동기화를 지키며 공유변수를 증가 시킨다.
	for(int i=0;i<COUNT_NUM;i++){
		local_cnt++;
	 	lock(sm,my_order);
		sm->critical_section_variable++;
		unlock(sm,my_order);
	}
	
	//자신이 증가시킨 횟수를 출력
	printf("child finish! local count=%d\n",local_cnt);

	//공유메모리를 뗀다.
	ret =shmdt(shmaddr);
	if(ret==-1) return 0;

	

	
}
