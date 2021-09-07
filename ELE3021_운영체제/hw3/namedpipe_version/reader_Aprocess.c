#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define PIPENAME "./named_pipe_file"


int main() {
	int ret;
	char msg[512];
	int fd;
	pid_t pid;

	//�ش� ������ count �� �޾ƿͼ� ���� 0�̶�� unlink�� ���� ���� ����
	ret = access(PIPENAME, F_OK);
	if (ret == 0) unlink(PIPENAME);


	//fifo ������ ������ - ������ ���� ���
	ret = mkfifo(PIPENAME, 0666);
	//���н� ���� �޼���
	if (ret < 0) {
		printf("creation of named pipe failed\n");
		return 0;
	}

	//�ش� �̸��� ���� ������ ����
	fd = open(PIPENAME, O_RDWR);
	//���н� ���� �޼���
	if (fd < 0) {
		printf("opening of named pipe failed\n");
		return 0;
	}


	//�ݺ������� �޼����� ����
	while (1) {
		//�޼��� �о����
		ret = read(fd, msg, sizeof(msg));
		//���н� ���� �޼���
		if (ret < 0) {
			printf("read failed\n");
			return 0;

		}
		//���� ����
		if(strcmp(msg,"q")==0) break; 
		//�ƴ϶�� �޼��� ���
		printf("%s\n", msg);
	}
	return 0;
}
