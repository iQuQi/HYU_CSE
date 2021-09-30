#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


struct msgbuf {
	long msgtype;
	char mtext[20];
};

int main() {
	//queue unique id
	key_t key_id;

	int i = 0;
	struct msgbuf rcvbuf;
	int msgtype = 3;
	
	//queue create
	//1111을 고유번호로 하는 큐를 생성한다. 이미 존재할 경우 해당하는 큐의 식별자 반환
	key_id = msgget((key_t)1111, IPC_CREAT | 0666);

	//실패시 에러 메시지 띄우기
	if (key_id == -1) {
		perror("msgget error: ");
		return 0;
	}

	//receive msg 
	while(1){
		//메시지를 받고 실패시 에러띄우기
		if (msgrcv(key_id, (void *)&rcvbuf, sizeof(struct msgbuf), msgtype, 0) == -1) {
			perror("msgrcv error: ");
		}
		
		else {
			//종료조건
			if(strcmp(rcvbuf.mtext,"q")==0) break;
			//종료 조건에 걸리지 않았다면 메세지 출력
			printf("%s\n", rcvbuf.mtext);
		}
	}

}
