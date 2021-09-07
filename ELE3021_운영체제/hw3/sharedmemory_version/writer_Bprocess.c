#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>



int main() {
	int shmid;
	void *shmaddr;
	int i,ret;

	//make a shared memory
	//key ���� ���� �����޸��� id�� ����
	shmid = shmget((key_t)1234, 1024, IPC_CREAT | 0666);
	//���н� ���� �޼���
	if (shmid == -1) {
		perror("shared memory access is failed\n");
		return 0;
	}

	//attach the shared memory
	//���μ����� �޸� ������ ���� �޸𸮸� ����
	shmaddr = shmat(shmid, (void *)0, 0);
	//���н� ���� �޼���
	if (shmaddr == (char*)-1) {
		perror("attach failed\n");
		return 0;
	}


	char input[50];
	//�ݺ������� ����ڿ��� input�� �޾Ƽ� �ش� ���� ���� �޸𸮿� ����.
	while(1){
		scanf("%[^\n]s",input);
		strcpy((char*)shmaddr, input);

		//���� ����
		if(strcmp(input,"q")==0) break;
		getchar();
	}


	//detach the shared memory
	ret = shmdt(shmaddr);
	//���н� ���� �޼���
	if (ret == -1) {
		perror("detach failed\n");
		return 0;
	}

}
