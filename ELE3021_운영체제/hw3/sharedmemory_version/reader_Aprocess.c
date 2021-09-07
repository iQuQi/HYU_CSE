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
	int ret;
	//get shared memory id
	//Ű ���� ���� ���� �޸��� id �޾ƿ���
	shmid = shmget((key_t)1234, 1024, IPC_CREAT | 0666);
	//���н� ���� �޼���
	if (shmid == -1) {
		perror("shared memory access is failed\n");
		return 0;
	}


	//attach the shared memory
	shmaddr = shmat(shmid, (void *)0, 0);
	//���н� ���� �޼���
	if (shmaddr == (char*)-1) {
		perror("attach failed\n");
		return 0;
	}

	//�޽����� �ݺ������� �о�´�
	char *read;
	while(1){
		//���� �޸𸮿��� ���� �о�´�
		read=(char *)shmaddr;
		//���� �ΰ��̶�� �Ѿ��
		if(strcmp("\0",read)==0) continue;

		//��������
		if(strcmp(read,"q")==0){
			 break;
		}

		//�޼��� ���
		printf("data read from shared memory: %s\n",read);
		//����� �޼����� �����Ѵ�
		strcpy((char*)shmaddr, "\0");
	}	
	
	//detach the shared memory
	ret = shmdt(shmaddr);
	//���н� ���� �޼���
	if (ret == -1) {
		perror("detach failed\n");
		return 0;
	}

	//���� �޸�(������)�� ���� ���ش�.
	ret = shmctl(shmid, IPC_RMID, 0);
	//���н� ���� �޽���
	if (ret == -1) {
		perror("removed failed\n");
		return 0;
	}

	return 0;
}
