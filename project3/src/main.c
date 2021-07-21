#include "bpt.h"
#include "file.h"

int main( int argc, char ** argv ) {
	int64_t in;
    char instruction;
	char * str= (char *)malloc(120); ;
	int ch,table_num,file_id,table_id;
	char * pathname=(char *)malloc(100);
	
	init_db(10);
	showTable();
	printf("please input pathname : ");
	scanf("%s",pathname);
	table_id=open_table(pathname);
	getchar();
	
	menu();
    	showTable();
	printf("\nMENU INPUT : ");


	if(table_id==FAIL) exit(1);
	
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'd'://delete ---key
            scanf("%" PRId64, &in);
			scanf("%d", &table_id);
			//printf("delete, in %" PRId64 "\n", in);
           		ch= db_delete(table_id,in);
			//printf("delete, ch %d\n", ch);
			//삭제 성공
			if (ch==SUCCESS) {
				printf("Delete Complete\n");
			}
			//삭제 실패시
			else {
				printf("Delete Failed\n");
			}
            break;

        case 'i'://insert -- key.val
            		scanf("%" PRId64 " %s", &in,str); 
			scanf("%d", &table_id);
			ch = db_insert(table_id,in, str);
			//중복없이 성공한경우
			if (ch==SUCCESS) {
				printf("Insertion Success\n");
			}
			//삽입 실패
			else {
				printf("Insertion Failed\n");
			}
			fflush(stdout);
            break;

        case 'f'://find --key
            scanf("%" PRId64, &in);
			scanf("%d", &table_id);
			ch = db_find(table_id,in,str);
			if (ch==SUCCESS) {
				printf("key: %" PRId64 ", Value: %s\n", in, str);
			}
			else {
				printf("Find Failed\n");
			}
			fflush(stdout);
            break;

        case 'q'://quit
	    shutdown_db();
            while (getchar() != (int)'\n');
            return 0;
            break;
		
        case 't'://tree print
	    scanf("%d", &table_id);
	    freeInfo(table_id);
	    headInfo(table_id);
       	    print_tree(table_id);
        break;

	case 's':
		shutdown_db();
		break;

	case 'c':
		scanf("%d", &table_id);
		close_table(table_id);
		break;
	case 'o':
		//새파일열기
		scanf("%s", pathname);
		//getchar();
		table_id=open_table(pathname);
		break;
	}
	showTable();
	menu();
	printf("\nMENU INPUT : ");
	
        while (getchar() != (int)'\n');
	
    }
    printf("\n");
    return 0;

}
