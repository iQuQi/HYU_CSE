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

	//해당 파일의 count 값 받아와서 만약 0이라면 unlink를 통해 삭제 진행
	ret = access(PIPENAME, F_OK);
	if (ret == 0) unlink(PIPENAME);


	//fifo 파일을 생성함 - 파이프 역할 대신
	ret = mkfifo(PIPENAME, 0666);
	//실패시 오류 메세지
	if (ret < 0) {
		printf("creation of named pipe failed\n");
		return 0;
	}

	//해당 이름을 가진 파이프 열기
	fd = open(PIPENAME, O_RDWR);
	//실패시 에러 메세지
	if (fd < 0) {
		printf("opening of named pipe failed\n");
		return 0;
	}


	//반복적으로 메세지를 읽음
	while (1) {
		//메세지 읽어오기
		ret = read(fd, msg, sizeof(msg));
		//실패시 오류 메세지
		if (ret < 0) {
			printf("read failed\n");
			return 0;

		}
		//종료 조건
		if(strcmp(msg,"q")==0) break; 
		//아니라면 메세지 출력
		printf("%s\n", msg);
	}
	return 0;
}
