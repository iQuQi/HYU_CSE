#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MSG_SIZE 80
#define PIPENAME "./named_pipe_file"


int main() {

	char msg[MSG_SIZE];
	int fd;
	int ret, i;
	char input[50];

	//�������� ����
	fd = open(PIPENAME, O_RDWR);
	//���нÿ� ���� �޼��� ����Ʈ
	if (fd < 0) {
		printf("opening of named pipe failed\n");
		return 0;
	}

	//�ݺ������� �޼����� ���´�.
	while(1)
	{
		//����ڿ��� �Է¹ޱ�
		sleep(1);
		scanf("%[^\n]s",input);

		//�ش� �Է��� �޼����� �����
		snprintf(msg, sizeof(msg), "%s", input);

		//���� �޼����� �������� ����.
		ret = write(fd, msg, sizeof(msg));
		//���н� ���� �޼��� ��ȯ
		if (ret < 0) {
			printf("write failed\n");
			return 0;
		}

		getchar();

		//���� ����
		if(strcmp(input,"q")==0) break;
	}

		
	
	return 0;
}
