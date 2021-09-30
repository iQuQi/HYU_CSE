#include <stdio.h>
#include <string.h>
#include <unistd.h>
int main(){

	FILE * f=fopen("text.txt","w");
	if(f==NULL) printf("fail to open\n");
	int i=0;
	while(i<12){
		sleep(1);
		fseek(f,0,SEEK_SET);

		fprintf(f,"%d\n",i);
		fflush(f);
		printf("Child1 wrote %d\n",i);

		i++;

	}

	fclose(f);
}
