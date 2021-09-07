

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
	key_t key_id;
	int i = 0;
	struct msgbuf sndbuf;

	//create queue
	//1111을 고유번호로 하는 메시지 큐를 생성 . 이미 큐가 있다면 해당하는 큐의 식별자 반환
	key_id = msgget((key_t)1111, IPC_CREAT | 0666);

	//실패시 에러 메세지 출력
	if (key_id == -1) {
		perror("msgget error: ");
		return 0;
	}

	//메시지 타입 설정
	sndbuf.msgtype = 3;

	//계속해서 input을 받아 메시지로 만들고 reader에게 전달
	while(1){
		//msg create
		char msg[20];
		scanf("%[^\n]s", msg);
		strcpy(sndbuf.mtext, msg);

		//msg send
		if (msgsnd(key_id, (void *)&sndbuf, sizeof(struct msgbuf), IPC_NOWAIT) == -1) {
			perror("msgsnd error: ");
		}

		getchar();

		//종료 조건
		if(strcmp(msg,"q")==0) break;
	}
}
