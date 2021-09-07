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
	//키 값을 통해 공유 메모리의 id 받아오기
	shmid = shmget((key_t)1234, 1024, IPC_CREAT | 0666);
	//실패시 오류 메세지
	if (shmid == -1) {
		perror("shared memory access is failed\n");
		return 0;
	}


	//attach the shared memory
	shmaddr = shmat(shmid, (void *)0, 0);
	//실패시 오류 메세지
	if (shmaddr == (char*)-1) {
		perror("attach failed\n");
		return 0;
	}

	//메시지를 반복적으로 읽어온다
	char *read;
	while(1){
		//공유 메모리에서 값을 읽어온다
		read=(char *)shmaddr;
		//만약 널값이라면 넘어간다
		if(strcmp("\0",read)==0) continue;

		//종료조건
		if(strcmp(read,"q")==0){
			 break;
		}

		//메세지 출력
		printf("data read from shared memory: %s\n",read);
		//출력한 메세지는 삭제한다
		strcpy((char*)shmaddr, "\0");
	}	
	
	//detach the shared memory
	ret = shmdt(shmaddr);
	//실패시 오류 메세지
	if (ret == -1) {
		perror("detach failed\n");
		return 0;
	}

	//공유 메모리(포인터)를 제거 해준다.
	ret = shmctl(shmid, IPC_RMID, 0);
	//실패시 오류 메시지
	if (ret == -1) {
		perror("removed failed\n");
		return 0;
	}

	return 0;
}
