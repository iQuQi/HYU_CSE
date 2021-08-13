#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define COUNT_NUM 2000000


//공유 메모리의 구조체틀
struct smStruct{
	int processidassign;
	int turn;
	int flag[2];
	int critical_section_variable;
};




int main() {
	int shmid;
	void * shmaddr;
	int ret;
	
	//구조체 크기 만큼의 공유메모리를 생성해준다
	shmid=shmget((key_t)1234,sizeof(struct smStruct),IPC_CREAT|0666);
	if(shmid == -1) return 0;

	
	//공유메모리를 자신의 메모리에 붙인다
	shmaddr=shmat(shmid,(void*)0,0);
	if(shmaddr==(char*)-1)return 0;

	//공유메모리의 주소를 sm 구조체 변수에 담아둔다
	struct smStruct * sm;
	sm=(struct smStruct *)shmaddr;

	//초기화 진행
	sm->turn=0;
	sm->flag[0]=0;
	sm->flag[1]=0;
	sm->critical_section_variable=0;

	//프로세스 생성 시작
	pid_t p1,p0;
	//첫 번째 프로세스 생성
	p0=fork();

	if (p0 == 0) {
		//자식이라면  다른 프로그램 덮어쓰기
		execl("./child", NULL);
		printf("1");
	}
	else {

		sm->processidassign = p0;
		p1 = fork();
		if (p1 == 0) {
			//두번째 자식이라면 다른 프로그램으로 덮어쓰기
			execl("./child", NULL);
			printf("2");
		}
	}


	//두 자식 프로세스가 끝날때까지 기다림
	int status1,status0;
	waitpid(p0,&status0,0);
	waitpid(p1,&status1,0);
	
	printf("Actual Count: %d | Expected Count: %d\n",sm->critical_section_variable,COUNT_NUM*2);

	
	//공유 메모리를 뗀다
	ret =shmdt(shmaddr);
	if(ret==-1) return 0;
	

	//공유메모리 제거
	ret =shmctl(shmid,IPC_RMID,0);
	if(ret==-1)return 0;

}
