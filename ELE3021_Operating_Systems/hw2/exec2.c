#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(){

	FILE *	f=fopen("text.txt","r");
	if(f==NULL) printf("fail to open\n");
	int i=0;
	char line[10];
	while(i<12){
	
		sleep(2);
		fflush(f);
		fseek(f,0,SEEK_SET);
		fgets(line,10,f);
		printf("%s",line);
		i+=2;
	}

		fclose(f);

}
