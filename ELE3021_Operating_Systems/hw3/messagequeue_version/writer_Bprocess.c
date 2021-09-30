

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
	//1111�� ������ȣ�� �ϴ� �޽��� ť�� ���� . �̹� ť�� �ִٸ� �ش��ϴ� ť�� �ĺ��� ��ȯ
	key_id = msgget((key_t)1111, IPC_CREAT | 0666);

	//���н� ���� �޼��� ���
	if (key_id == -1) {
		perror("msgget error: ");
		return 0;
	}

	//�޽��� Ÿ�� ����
	sndbuf.msgtype = 3;

	//����ؼ� input�� �޾� �޽����� ����� reader���� ����
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

		//���� ����
		if(strcmp(msg,"q")==0) break;
	}
}
