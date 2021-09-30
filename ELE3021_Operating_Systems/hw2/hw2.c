#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

	pid_t pid_1,pid_2;
	printf("Waiting For Child Process\n");
	int st1,st2;
	
	pid_1=fork();
	if(pid_1==0) execl("./exec1",NULL);
	
	pid_2=fork();	
	if(pid_2==0) execl("./exec2",NULL);
	waitpid(pid_1,&st1,0);
	waitpid(pid_2,&st2,0);
	printf("Child processes are exit (%d) (%d)\n",st1,st2);






}

