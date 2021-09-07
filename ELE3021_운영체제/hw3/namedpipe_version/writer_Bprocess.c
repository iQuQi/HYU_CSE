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

	//파이프를 연다
	fd = open(PIPENAME, O_RDWR);
	//실패시에 에러 메세지 프린트
	if (fd < 0) {
		printf("opening of named pipe failed\n");
		return 0;
	}

	//반복적으로 메세지를 적는다.
	while(1)
	{
		//사용자에게 입력받기
		sleep(1);
		scanf("%[^\n]s",input);

		//해당 입력을 메세지로 만들기
		snprintf(msg, sizeof(msg), "%s", input);

		//만든 메세지를 파이프에 쓴다.
		ret = write(fd, msg, sizeof(msg));
		//실패시 에러 메세지 반환
		if (ret < 0) {
			printf("write failed\n");
			return 0;
		}

		getchar();

		//종료 조건
		if(strcmp(input,"q")==0) break;
	}

		
	
	return 0;
}
