#include "lock_manager.h"
#include "file.h"
#include "trx_manager.h"
#include "buf_manager.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define TRANSFER_THREAD_NUMBER	(8)
#define SCAN_THREAD_NUMBER		(1)

#define TRANSFER_COUNT			(1000000)
#define SCAN_COUNT				(1000000)

#define TABLE_NUMBER			(3)
#define RECORD_NUMBER			(5)
#define INITIAL_MONEY			(100000)
#define MAX_MONEY_TRANSFERRED	(100)
#define SUM_MONEY				(TABLE_NUMBER * RECORD_NUMBER * INITIAL_MONEY)


int cnt;
int action;
/*
 * This thread repeatedly transfers some money between accounts randomly.
 */
void*
transfer_thread_func(void* arg)
{
	int trx_id=trx_begin();
	cnt++;
	
	//printf("\nmy trx id is %d-----------------------------------------\n",trx_id);
	int ran;
	char * tmp=(char*)malloc(120);
	int64_t key_ran;

	for (int i = 0; i < 15; i++)
	{
		ran = rand()%2;
		key_ran = rand() % 15;
		if (ran == 0) {
			//printf("trx[%d] doing find : key = %ld-------------------------\n",trx_id,key_ran);
			ch=db_find(1,key_ran,tmp,trx_id);
			if (ch == SUCCESS) {
				//printf("key: %" PRId64 ", Value: %s\n", key_ran, tmp);
			}
			else if (ch == ABORT) {
				
				//printf("Find Conflict Arise and Abort\n");
				
			}
			else if(ch==FAIL) {
				//printf("Find Failed\n");
			}			
			//printf("-----------------------------------------\n");
		}
		else {
			//printf("trx[%d] doing update : key = %ld--------------------------\n",trx_id,key_ran);
			ch=db_update(1, key_ran, "test1", trx_id);

			if (ch == SUCCESS) {
				//printf("Update success\n");
			}
			else if (ch == ABORT) {
				
				//printf("Update Conflict Arise and Abort\n");
				
			}
			else if(ch==FAIL) {
				//printf("Update Failed\n");
			}
			//printf("-----------------------------------------\n");
		}
		
		
	}
	
	trx_commit(trx_id);
	cnt--;
	
//printf("[%d] trx is done !!!------------------------------------------\n",trx_id);
//printf("now exist thread is %d\n",cnt);
	return NULL;
}

/*
 * This thread repeatedly check the summation of all accounts.
 * Because the locking strategy is 2PL (2 Phase Locking), the summation must
 * always be consistent.
 */


int main()
{
	pthread_t	transfer_threads[100];
	char * pathname=(char *)malloc(100);
	printf("please input pathname : ");
	int a;
	
	scanf("%s",pathname);
	int table_id = open_table(pathname);
	if (table_id == FAIL) exit(1);
	srand(time(NULL));
	for(int i =0;i<15;i++){
	a=rand()%100;
	//printf("a = %d",a);
	db_insert(1,a,"hihi");
	//printf("insert clear\n");
	}

	//printf("what\n");
	/* thread create */
	for (int i = 0; i < 100; i++) {
		pthread_create(&transfer_threads[i], 0, transfer_thread_func, NULL);
	}


	/* thread join */
	for (int i = 0; i < 100; i++) {
	//printf("exit suc one ");
		pthread_join(transfer_threads[i], NULL);
	}
	printf("\nall clear\n");

	//print_tree(1);
	
	
	

	return 0;
}

