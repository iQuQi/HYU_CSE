#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define COUNT_NUM 2000000


//���� �޸��� ����üƲ
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
	
	//����ü ũ�� ��ŭ�� �����޸𸮸� �������ش�
	shmid=shmget((key_t)1234,sizeof(struct smStruct),IPC_CREAT|0666);
	if(shmid == -1) return 0;

	
	//�����޸𸮸� �ڽ��� �޸𸮿� ���δ�
	shmaddr=shmat(shmid,(void*)0,0);
	if(shmaddr==(char*)-1)return 0;

	//�����޸��� �ּҸ� sm ����ü ������ ��Ƶд�
	struct smStruct * sm;
	sm=(struct smStruct *)shmaddr;

	//�ʱ�ȭ ����
	sm->turn=0;
	sm->flag[0]=0;
	sm->flag[1]=0;
	sm->critical_section_variable=0;

	//���μ��� ���� ����
	pid_t p1,p0;
	//ù ��° ���μ��� ����
	p0=fork();

	if (p0 == 0) {
		//�ڽ��̶��  �ٸ� ���α׷� �����
		execl("./child", NULL);
		printf("1");
	}
	else {

		sm->processidassign = p0;
		p1 = fork();
		if (p1 == 0) {
			//�ι�° �ڽ��̶�� �ٸ� ���α׷����� �����
			execl("./child", NULL);
			printf("2");
		}
	}


	//�� �ڽ� ���μ����� ���������� ��ٸ�
	int status1,status0;
	waitpid(p0,&status0,0);
	waitpid(p1,&status1,0);
	
	printf("Actual Count: %d | Expected Count: %d\n",sm->critical_section_variable,COUNT_NUM*2);

	
	//���� �޸𸮸� ����
	ret =shmdt(shmaddr);
	if(ret==-1) return 0;
	

	//�����޸� ����
	ret =shmctl(shmid,IPC_RMID,0);
	if(ret==-1)return 0;

}
