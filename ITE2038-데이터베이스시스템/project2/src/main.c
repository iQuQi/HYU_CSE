#include "bpt.h"
#include "file.h"

int main( int argc, char ** argv ) {
	int64_t in;
    	char instruction;
	char * str= (char *)malloc(120); ;
	int ch;
	char * pathname=(char *)malloc(100);
	
	printf("please input pathname : ");
	scanf("%s",pathname);
	int fd=open_table(pathname);
	getchar();
	menu();

	if(fd==-1) exit(1);
	
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'd'://delete ---key
            scanf("%" PRId64, &in);
			//printf("delete, in %" PRId64 "\n", in);
           		 ch= db_delete(in);
			//printf("delete, ch %d\n", ch);
			//삭제 성공
			if (!ch) {
				printf("delete complete\n");
			}
			//삭제 실패시
			else {
				printf("not found : delete failed\n");
			}
            break;
        case 'i'://insert -- key.val
            scanf("%" PRId64 " %s", &in,str); 
			ch = db_insert(in, str);
			//중복없이 성공한경우
			if (!ch) {
				printf("insertion success\n");
			}
			//삽입 실패
			else {
				printf("key already exists : Insertion failed\n");
			}
			fflush(stdout);
            break;

        case 'f'://find --key
            scanf("%" PRId64, &in);
			ch = db_find(in,str);
			if (!ch) {
				printf("key: %" PRId64 ", value: %s\n", in, str);
	
			}
			else {
				printf("not found\n");
			}
			fflush(stdout);
            break;
        case 'q'://quit
	    close_table(fd);
            while (getchar() != (int)'\n');
            return 0;
            break;
		
        case 't'://tree print
            print_tree();
	    
            break;
        }
	menu();
        while (getchar() != (int)'\n');
    }
    printf("\n");
    return 0;
}
