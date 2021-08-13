#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define COUNT_NUM 2000000

//�����޸𸮿� �� ����ü
struct smStruct{
	int processidassign;
	int turn;
	int flag[2];
	int critical_section_variable;
};

//lock�� �ɾ��ִ� �Լ�
void lock(struct smStruct * sm,int myorder) {
	    //�ڽ��� flag�� 1�� �����ϰ� ��뿡�� ���� �ѱ��
		sm->flag[myorder]=1;
		sm->turn=1-myorder;
		//��밡 critical ���� �ִ� ���� ��ٸ���.
		while (sm->flag[1-myorder]&&sm->turn==1 - myorder);
}

//lock�� �����ϴ� �Լ�
void unlock(struct smStruct * sm,int myorder) {
	//�ڽ��� flag�� 0���� ����
	sm->flag[myorder]=0;
	
}


int main(){
	//�ڽ��� pid�� �޾ƿ���
	pid_t pid = getpid();

	int my_order=0;
	int shmid;
	void * shmaddr;
	int ret;

	
	

	//���� �޸𸮸� ����/�̹� �ִٸ� ��������
	int local_cnt=0;
	shmid=shmget((key_t)1234,sizeof(struct smStruct),IPC_CREAT|0666);
	if(shmid == -1) return 0;

	//�����޸𸮸� �ڽ��� �ּ� ������ ����
	shmaddr=shmat(shmid,(void*)0,0);
	if(shmaddr==(char*)-1)return 0;



	//sm �����ȿ� �����޸��� ������ �����͵д�.
	struct smStruct * sm;
	sm=(struct smStruct *)shmaddr;


	//�����޸𸮿� ����Ǿ��ִ� ���� ���� �� ���μ����� �ٸ� order�� �ο��Ѵ�
	if (pid == sm->processidassign)my_order = 0;
	else my_order = 1;

	// �ڽ��� flag �迭������ �ε����� ���
	printf("my order is %d, pid is %d\n", my_order, pid);

	//����ȭ�� ��Ű�� ���������� ���� ��Ų��.
	for(int i=0;i<COUNT_NUM;i++){
		local_cnt++;
	 	lock(sm,my_order);
		sm->critical_section_variable++;
		unlock(sm,my_order);
	}
	
	//�ڽ��� ������Ų Ƚ���� ���
	printf("child finish! local count=%d\n",local_cnt);

	//�����޸𸮸� ����.
	ret =shmdt(shmaddr);
	if(ret==-1) return 0;

	

	
}
