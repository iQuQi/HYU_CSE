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
	//1111�� ������ȣ�� �ϴ� ť�� �����Ѵ�. �̹� ������ ��� �ش��ϴ� ť�� �ĺ��� ��ȯ
	key_id = msgget((key_t)1111, IPC_CREAT | 0666);

	//���н� ���� �޽��� ����
	if (key_id == -1) {
		perror("msgget error: ");
		return 0;
	}

	//receive msg 
	while(1){
		//�޽����� �ް� ���н� ��������
		if (msgrcv(key_id, (void *)&rcvbuf, sizeof(struct msgbuf), msgtype, 0) == -1) {
			perror("msgrcv error: ");
		}
		
		else {
			//��������
			if(strcmp(rcvbuf.mtext,"q")==0) break;
			//���� ���ǿ� �ɸ��� �ʾҴٸ� �޼��� ���
			printf("%s\n", rcvbuf.mtext);
		}
	}

}
