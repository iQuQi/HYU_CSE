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
	//key 값을 통해 공유메모리의 id를 얻어옴
	shmid = shmget((key_t)1234, 1024, IPC_CREAT | 0666);
	//실패시 오류 메세지
	if (shmid == -1) {
		perror("shared memory access is failed\n");
		return 0;
	}

	//attach the shared memory
	//프로세스의 메모리 공간에 공유 메모리를 붙임
	shmaddr = shmat(shmid, (void *)0, 0);
	//실패시 오류 메세지
	if (shmaddr == (char*)-1) {
		perror("attach failed\n");
		return 0;
	}


	char input[50];
	//반복적으로 사용자에게 input을 받아서 해당 값을 공유 메모리에 쓴다.
	while(1){
		scanf("%[^\n]s",input);
		strcpy((char*)shmaddr, input);

		//종료 조건
		if(strcmp(input,"q")==0) break;
		getchar();
	}


	//detach the shared memory
	ret = shmdt(shmaddr);
	//실패시 오류 메세지
	if (ret == -1) {
		perror("detach failed\n");
		return 0;
	}

}
