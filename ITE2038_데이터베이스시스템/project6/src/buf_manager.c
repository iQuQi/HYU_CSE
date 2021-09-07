
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<inttypes.h>//PRid64 - int64_t를 정수로 출력
#include<sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include "file.h"
#include "buf_manager.h"
#include "lock_manager.h"
#include "trx_manager.h"
#include "log_manager.h"





//여기서 버퍼락 페이지락 버퍼언락 얼어나게
int find_page(int tableid, int64_t key,int trx_id,int mode) {

//printf("\nfind page[%d] 1\n",trx_id);
	int i = 0;
	//헤더를 받아온다 -> 이 안에서 버퍼락 -> 페이지찾고 페이지락 -> 버퍼언락
	int head = pageScan(tableid, 0);
	pagenum_t rootp=b_head.root_page;
	clearPin(head);
	pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
	//printf("find page[%d] : page unlock head [%d] \n", trx_id, head );


	//빈트리 일경우
	if (rootp == 0) {
//printf("find page[%d] - empty\n", trx_id);
		return FAIL;
	}

	//trx 테이블에서 확인
	if (!trx_find(trx_id)) {
		//printf("find page[%d]: not exist trx\n", trx_id);
		return FAIL;
	}

	//루트정보저장
	int trace = pageScan(tableid, rootp);
	lock_t * tmp_l;
//printf("find page[%d] : trace page lock\n", trx_id);
	pagenum_t tmp = rootp;

	//여기는 안해줘도 될듯 
	while (!b_M.frameArray[trace].frame_p.is_leaf) {
		i=0;
		while (i < b_M.frameArray[trace].frame_p.num_key) {
		clearPin(trace);
		pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
		//printf("find page[%d] : page unlock [%d] before acquire lock \n",trx_id,trace);
		tmp_l=lock_acquire(tableid,b_M.frameArray[trace].frame_p.branch[i].key,trx_id,mode);
		setPin(trace);
		pthread_mutex_lock(&b_M.frameArray[trace].page_latch);
		//printf("find page[%d] : page lock [%d] after acquire lock \n",trx_id,trace);
		if(tmp_l==NULL){
		//	printf("find_page[%d] : lock acquire failed\n", trx_id);
			clearPin(trace);
			
			pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
//printf("find page[%d] : page unlock inside [%d] \n", trx_id,trace);
			
			return FAIL;
		}
			if (key >= b_M.frameArray[trace].frame_p.branch[i].key) {
				if (i == (b_M.frameArray[trace].frame_p.num_key - 1)) {
					
					break;
				}
				i++;
			}
			else {
				i--;
				
				break;
			}
		}

		if (i != -1) {
//printf("find page[%d] - 2\n", trx_id);
			tmp = b_M.frameArray[trace].frame_p.branch[i].child;
			clearPin(trace);
			pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
//printf("find page[%d] : page unlock inside [%d] \n", trx_id,trace);
			trace = pageScan(tableid, tmp);
			
		}
		else if (i == -1) {
//printf("find page[%d] - 3\n", trx_id);
			tmp = b_M.frameArray[trace].frame_p.right_left;
			clearPin(trace);
			pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
//printf("find page[%d] : page unlock inside [%d] \n", trx_id,trace);
			trace = pageScan(tableid, tmp);
			
		}

	}
	clearPin(trace);
	pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
	//printf("find page[%d] : page unlock1 [%d] \n", trx_id,trace);
	//헤더를 받아온다 -> 이 안에서 버퍼락 -> 페이지찾고 페이지락 -> 버퍼언락
	int find = pageScan(tableid, tmp);
//printf("find page[%d] - 4\n", trx_id);

	clearPin(find);
	pthread_mutex_unlock(&b_M.frameArray[find].page_latch);
	//printf("find page[%d] : page unlock3 [%d] \n", trx_id,find);
	//여기 페이지언락 3개

	//끝날시점에는 모든 mutex들이 해제

	return find;
}

int db_find(int table_id, int64_t key, char * ret_val, int trx_id)
{
	//if(b_M.frameArray==NULL) init_db(500);
	
	int i;
	if (b_M.table[table_id - 1].isopen == 0) {
	//	printf("File Is Closed\n");
		return SUCCESS;
	}

	//trx 테이블에서 확인
	if (!trx_find(trx_id)) {
	//	printf("db find[%d] : not exist trx\n", trx_id);
		return SUCCESS;
	}


//printf("\ndb_find[%d] start -1\n",trx_id);
		//그 키가 들어있을수도 있는 리프페이지를 가져온다
	
	int find_p = find_page(table_id, key,trx_id,0);
	lock_t * tmp_l;
	//버퍼락 
	
//printf("db find page lock [%d] suc\n",find_p);
	//버퍼언락
	
	//빈트리인 경우
	if (find_p == FAIL) {
//printf("db_find[%d]- empty\n",trx_id);
		
//printf("db find[%d] : page unlock-fail [%d] \n",trx_id,find_p);
		return SUCCESS;
	}

	setPin(find_p);
	pthread_mutex_lock(&b_M.frameArray[find_p].page_latch);

	//동일페이지 내에서 임
	//키를 발견하면 멈춘다 = 페이지언락->레코드락 ->페이지락->읽기->레코드언락(얘는 함) --- 다음 레코드로 이거 반복(abort인지 확인),
	for (i = 0; i < b_M.frameArray[find_p].frame_p.num_key; i++)
	{
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
		//printf("db_find[%d] : page unlock-1 [%d] before acquire lock \n",trx_id,find_p);
		tmp_l=lock_acquire(table_id,b_M.frameArray[find_p].frame_p.record[i].key,trx_id,0);
		setPin(find_p);
		pthread_mutex_lock(&b_M.frameArray[find_p].page_latch);
		//printf("db find[%d] : page lock [%d] after acquire lock \n",trx_id,find_p);
		if(tmp_l==NULL){
		//	printf("db find : lock acquire failed-1\n");
			clearPin(find_p);
			pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("db find[%d] : page unlock-0 [%d] \n",trx_id,find_p);
			return ABORT;
		}
		if (b_M.frameArray[find_p].frame_p.record[i].key == key) {
			
			break;
		}
		
	}
//printf("db_find[%d]-2\n",trx_id);
		//인덱스가 끝번이 아니라면 찾지못한것 - break로 멈춰진게 아니라는 뜻
	if (i == b_M.frameArray[find_p].frame_p.num_key) {
//printf("db_find[%d] end----- not find\n",trx_id);
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("db find[%d] : page unlock-1 [%d] \n",trx_id,find_p);
		return SUCCESS;
	}
	else {
//printf("db_find end[%d]------ find\n",trx_id);
//이거전에도 레코드락 작업 S모드로 (abort확인) -> 레코드 언락 안함-> commit시에 함
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
		//printf("db_find[%d] : page unlock-2 [%d] before acquire lock \n",trx_id,find_p);
		tmp_l=lock_acquire(table_id,b_M.frameArray[find_p].frame_p.record[i].key,trx_id,0);
		setPin(find_p);
		pthread_mutex_lock(&b_M.frameArray[find_p].page_latch);
		//printf("db find[%d] : page lock [%d] after acquire lock \n",trx_id,find_p);
		if(tmp_l==NULL){
			//printf("db find : lock acquire failed-2\n");
			clearPin(find_p);
			pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("db find[%d] : page unlock-2 [%d] \n",trx_id,find_p);
			return ABORT;
		}

//printf("db find[%d] : find val=%s\n",trx_id,b_M.frameArray[find_p].frame_p.record[i].val);
		strcpy(ret_val, b_M.frameArray[find_p].frame_p.record[i].val);
//printf("hoo.....\n");
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("db find[%d] : page unlock [%d] FINISH \n",trx_id,find_p);
		return SUCCESS;
	}
	
}


//키에 해당하는 페이지를 읽어와서 레코드 수정 
//성공시 0리턴 
//실패시 nonzero -> abort 필요 -> 리턴 전에 release 다하고 undo
//->E 모드, 앞에 누가 잡고 있다면 무조건 기다림
//Abort였다면 ABORT리턴
int db_update(int table_id, int64_t key, char * val, int trx_id)
{

	//if(b_M.frameArray==NULL) init_db(500);
	int i;
	char * str_tmp= (char *)malloc(120);
	//printf("\ndb update start[%d]\n",trx_id);
	//find를 해서 해당 페이지를 읽어옴-E모드로
	//무사히 끝났다는건 데드락이 일어나지 않음을 의미하고 또한 이 lock이 wakeup한 상태라는 것임
	//->이함수 끝나기전에 해제할거 다해줘야함
	if (b_M.table[table_id - 1].isopen == 0) {
		printf("File Is Closed\n");
		return SUCCESS;
	}

	//trx 테이블에서 확인
	if (!trx_find(trx_id)) {
		//printf("db update : not exist trx\n");
		return SUCCESS;
	}
	
//printf("db update[%d] 1\n",trx_id);
	int f_c = db_find(table_id, key, str_tmp, trx_id);
	
	if (f_c == ABORT) {return ABORT;}

//printf("db update[%d] 2\n",trx_id);
	int find_p = find_page(table_id, key,trx_id,0);
	
	//버퍼락 페이지락 버퍼언락
	
	if (find_p == ABORT) {return ABORT;}
	else if (find_p == FAIL) {return SUCCESS;}
//printf("db update[%d] 3\n",trx_id);

	setPin(find_p);
	pthread_mutex_lock(&b_M.frameArray[find_p].page_latch);
//printf("db update [%d] lock page [%d]\n",trx_id,find_p);
	lock_t * tmp_l;
	for (i = 0; i < b_M.frameArray[find_p].frame_p.num_key; i++)
	{
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
		//printf("db update[%d] : page unlock-1 [%d] before acquire lock \n",trx_id,find_p);
		tmp_l=lock_acquire(table_id,b_M.frameArray[find_p].frame_p.record[i].key,trx_id,0);
		setPin(find_p);
		pthread_mutex_lock(&b_M.frameArray[find_p].page_latch);
		//printf("db update[%d] : page lock [%d] after acquire lock \n",trx_id,find_p);
		if(tmp_l==NULL){
			//printf("db update : lock acquire failed-1\n");
			clearPin(find_p);
			pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("db update[%d] : page unlock [%d] FINISH \n",trx_id,find_p);
//printf("abort2\n");
			return ABORT;
		}	
		if (b_M.frameArray[find_p].frame_p.record[i].key == key) {
			
			break;
				
		}
	}

		//인덱스가 끝번이 아니라면 찾지못한것 - break로 멈춰진게 아니라는 뜻
	if (i == b_M.frameArray[find_p].frame_p.num_key) {
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
		//printf("not find the date\n")
		//printf("db find[%d] : page unlock-1 [%d] \n",trx_id,find_p);
		return SUCCESS;
	}



//printf("db_update[%d]: find end------ find, pre val=%s\n",trx_id,b_M.frameArray[find_p].frame_p.record[i].val);
//이거전에도 레코드락 작업 S모드로 (abort확인) -> 레코드 언락 안함-> commit시에 함
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
		//printf("db update[%d] : page unlock-2 [%d] before acquire lock \n",trx_id,find_p);
		tmp_l=lock_acquire(table_id,b_M.frameArray[find_p].frame_p.record[i].key,trx_id,1);
		setPin(find_p);
//printf("seg?..\n");
		pthread_mutex_lock(&b_M.frameArray[find_p].page_latch);
		//printf("db update[%d] : page lock [%d] after acquire lock \n",trx_id,find_p);
		if(tmp_l==NULL){
			//printf("db update : lock acquire failed-2\n");
			clearPin(find_p);
			pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("db update[%d] : page unlock [%d] FINISH \n",trx_id,find_p);
			//printf("abort3\n");
			return ABORT;
		}
		
		char *old=(char*)malloc(120);
		tmp_l->change = 1;
		strcpy(old, b_M.frameArray[find_p].frame_p.record[i].val);
		strcpy(b_M.frameArray[find_p].frame_p.record[i].val,val);

		//업데이트 로그 발생
	//int log_update(int trx_id,int table_id,int64_t page_num,char* old_image,char* new_image);
		int LSN=log_update(trx_id, table_id, find_p, old, val, i);
		b_M.frameArray[find_p].frame_p.page_LSN = LSN;
		

	//lock해제는 안함 commit 때까지 기다림
	//printf("db update[%d] SUCCESS end, change val=%s\n",trx_id,val);
	clearPin(find_p);
	pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("dp update[%d] : mutex unlock [%d] FINISH \n",trx_id,find_p);


	
	
	
	return SUCCESS;
}




//5번할일 아래 함수 다 수정하기

//함수 성공시 0리턴 실패시 -1 리턴
int ch;

//수정 : page lock 상태 확인할 수 있게
void bufInfo() {
	//if(b_M.frameArray==NULL) init_db(500);
	printf("\n<buffer info>\n");
	printf("table use : %d\n", b_M.table_use);
	printf("table total : %d\n", b_M.table_total);
	printf("use num : %d\n", b_M.use_num);
	printf("LRU head : %d\n", b_M.LRU_head);
	printf("LRU tail : %d\n", b_M.LRU_tail);

	for (int i = 0; i < b_M.frame_capacity; i++) {
		printf("%d : tableid,pagenum = (%d,%ld), next = %d, pre = %d \n", i, b_M.frameArray[i].table_id, b_M.frameArray[i].page_num,
			b_M.frameArray[i].next, b_M.frameArray[i].pre);
		printf("ispinned=%d, isdirty=%d\n", b_M.frameArray[i].ispinned, b_M.frameArray[i].isdirty);

		if (b_M.frameArray[i].page_num != 0) {
			printf("parent = %ld / is leaf = %d / num key = %d / right_left = %ld\n",
				b_M.frameArray[i].frame_p.parent, b_M.frameArray[i].frame_p.is_leaf,
				b_M.frameArray[i].frame_p.num_key, b_M.frameArray[i].frame_p.right_left);
		}
		else if (b_M.frameArray[i].table_id != 0) {
			printf("it is head\n");
		}
		else printf("not used\n\n");
	}


}


//pin 관련 정보 수정
void freeInfo(int table_id) {
	//if(b_M.frameArray==NULL) init_db(500);
	printf("\n<free page> \n");
	int head = pageScan(table_id, 0);
	int free = pageScan(table_id, b_head.free_page);


	int cnt = 0;
	pagenum_t tmp_next = b_head.free_page;
	if (b_head.free_page == 0) printf("no free page\n");
	else {
		while (1) {
			if (cnt > 20) break;
			cnt++;
			printf("free %d : pagenum = %ld, right=%ld \n", cnt, tmp_next,
				b_M.frameArray[free].frame_p.parent);
			tmp_next = b_M.frameArray[free].frame_p.parent;
			if (tmp_next == 0)break;
			clearPin(free);
			pthread_mutex_unlock(&b_M.frameArray[free].page_latch);
printf("page unlock [%d]\n",free);
			free = pageScan(table_id, tmp_next);
			

		}
	}

	clearPin(free);
	clearPin(head);
pthread_mutex_unlock(&b_M.frameArray[free].page_latch);
printf("page unlock [%d]\n",free);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
printf("page unlock [%d]\n",head);

	//페이지언락
}


//핀 정보 수정
void headInfo(int tableid) {
	//if(b_M.frameArray==NULL) init_db(500);
	int head = pageScan(tableid, 0);
	
	printf("\n<header info>\n");
	printf("root , free, pagenum = %ld, %ld, %ld\n", b_head.root_page, b_head.free_page, b_head.page_num);
	clearPin(head);
	pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
printf("page unlock [%d]\n",head);

}

//핀 관련 수정
int close_table(int table_id) {
//printf("\nclosetable1 -- frame capacity : %d\n",b_M.frame_capacity);
	if (table_id > 10 || table_id < 1) {
//printf("wrong close\n");
		return FAIL;
	}

	//그 테이블의 페이지들을 전부 디스크로 내려준다.
	for (int i = 0; i < b_M.frame_capacity; i++)
	{
//printf("closetable2 -- cmp > framArray[i].table id / close id = %d/%d\n",b_M.frameArray[i].table_id , table_id);

		if (b_M.frameArray[i].table_id == table_id) {
			if (b_M.frameArray[i].ispinned == 1) {
				clearPin(i);
				pthread_mutex_unlock(&b_M.frameArray[i].page_latch);
//printf("page unlock [%d]\n",i);
			}
			pageDrop(i);
		}
	}

	//테이블 배열에서 없애준다
	for (int i = 0; i < 10; i++)
	{
//printf("closetable3\n");
		if (b_M.table[i].id == table_id) {
			b_M.table[i].isopen = 0;
			b_M.table_use--;
			close(b_M.table[i].fd);
		}

	}
	//clear the trx table

	//clear the lock table

	//not need?....
	
	return SUCCESS;
}

//수정 x
int shutdown_db()
{
	//테이블에 올라와있는 개수만큼 close table
	for (int i = 0; i < 10; i++)
	{
		close_table(b_M.table[i].id);
	}

	return SUCCESS;
}

////////////////////////////////////////////


//수정x
int init_db(int buf_num,int flag, int log_num,char* log_path, char* logmsg_path)
{
//printf("\ninit db 1\n");
		//동적할당 시켜준다, calloc은 모두 0을 초기화... 그래도 혹시 모르니ㅠ
	b_M.frameArray = (buffer_S*)calloc(buf_num, sizeof(struct bufferStructure));
	for (int i = 0; i < 10; i++)
	{
		b_M.table[i].fd = 0;
		b_M.table[i].id = 0;
		b_M.table[i].path = (char*)calloc(20, sizeof(char));
		b_M.table[i].isopen = 0;


	}
	b_M.table_total = 0; 
	b_M.table_use = 0;
	b_M.frame_capacity = buf_num;
	b_M.use_num = 0;

	b_M.LRU_head = -1;
	b_M.LRU_tail = -1;
	pthread_mutex_init(&buf_latch, NULL);
	

	for (int i = 0; i < buf_num; i++)
	{
		b_M.frameArray[i].pre = -1;
		b_M.frameArray[i].next = -1;
		pthread_mutex_init(&b_M.frameArray[i].page_latch, NULL);

	}

	open_log(log_path);
	open_msg_log(logmsg_path);
	init_log_buf(1000);

	if (flag == 0) {//노말
		//로그 파일에 있는 모든 로그레코드를 실행
		//분석->redo->undo를 모두 진행하도록 한다
		
		//분석
		log_M.log_now=analysis();//로그파일을 보면서 루저를 찾는다.

		//redo
		recovery_redo(0);//모든 로그를 재실행

		//undo
		recovery_undo(0);//루저에 대해서 undo 실행

	}
	else if (flag == 1) {//redo crash
		//redo 단계에서 주어진 로그 seq num까지 모두 진행하고 

		//분석
		log_M.log_now=analysis();

		//redo
		recovery_redo(log_num);
	}
	else if (flag == 2) {//undo crash
		//undo단계에서 주어진  로그 seq num까지 무두 진행하고 

		//분석
		log_M.log_now=analysis();

		//redo
		recovery_redo(0);

		//undo
		recovery_undo(log_num);
	}

	//로그 버퍼와 페이지 버퍼의 dirty를 전부 디스크로 내려준다

	return SUCCESS;
}


//핀 수정
int open_table(char *pathname) {
	
	//if(b_M.frameArray==NULL) init_db(500);
	//헤더를 읽어온다.
	int root;
	int head;
	//O_creat 모드에서는 세번째 인자에 파일에 접근권한 설정가능
	//파일이 없는 경우 새로 생성해주는데 파일이 존재한다면 에러를 리턴(O_EXCL)


	//같은 파일명이 존재하는지 체크
	int i;
	for (i = 0; i < 10; i++)
	{
		if (!strcmp(pathname, b_M.table[i].path)) {
			break;
		}
	}

//printf("open table : i= %d\n",i);
		//이전에 열린적이 없던 테이블
	if (i == 10) {
		if (b_M.table_total == 10) {
			printf("full table\n");
			return FAIL;
		}

//printf("open table : first open-1\n");
		i = b_M.table_total;
		b_M.table[i].fd = open(pathname, O_RDWR | O_CREAT | O_EXCL, 0777);
		int table_fd = b_M.table[i].fd;
		int table_id;
		//정상적으로 열렸다면
		if (table_fd > 0) {
//printf("open table : first open-2\n");

			b_M.table[i].id = i + 1;
			table_id = b_M.table[i].id;
			strcpy(b_M.table[i].path, pathname);
			b_M.table[i].isopen = 1;
			b_M.table_total++;
			b_M.table_use++;

//printf("head scan : %d\n",table_id);
			head = pageScan(table_id, 0);
			//헤더 페이지 개수 카운트
			b_head.page_num = 1;
			b_head.free_page = 0;
			b_head.root_page = 0;

			//헤더 dirty/pin set
			clearPin(head);
			pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("page unlock [%d]\n",head);
			setDirty(head);

			return table_id;
		}

		//생성되어있다면 다시 읽어온다.
		b_M.table[i].fd = open(pathname, O_RDWR);
		table_fd = b_M.table[i].fd;

		//정상으로 읽어진경우
		if (table_fd > 0) {

//printf("open table : first open-3\n");
			b_M.table[i].id = i + 1;
			table_id = b_M.table[i].id;
			head = pageScan(table_id, 0);

			strcpy(b_M.table[i].path, pathname);
			b_M.table[i].isopen = 1;
			b_M.table_total++;
			b_M.table_use++;
//printf("open table : first open-4\n");
						//루트페이지를 버퍼에 올려준다
			if (b_head.root_page != 0) {
//printf("open table : first open-5\n");
				root = pageScan(table_id, b_head.root_page);
				clearPin(root);
				pthread_mutex_unlock(&b_M.frameArray[root].page_latch);
//printf("page unlock [%d]\n",root);
			}
//printf("open table : first open-6\n");
			clearPin(head);
			pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("page unlock [%d]\n",head);
			return table_id;
		}


	}

	//이전에 열린적이 있어 정보가 저장되어있는 경우 fd만 갱신해서 저장
	else {
//printf("open table : already open-1\n");
		b_M.table[i].fd = open(pathname, O_RDWR);
//printf("new fd : %d\n",b_M.table[i].fd);
		int table_fd = b_M.table[i].fd;
		int table_id = b_M.table[i].id;

		if (table_fd > 0) {
//printf("open table : already open-2\n");
						//헤더를 버퍼에 올려준다
			head = pageScan(table_id, 0);
			b_M.table[i].isopen = 1;
			b_M.table_use++;

			//루트페이지를 버퍼에 올려준다
			if (b_head.root_page != 0) {
				root = pageScan(table_id, b_head.root_page);
				clearPin(root);
				pthread_mutex_unlock(&b_M.frameArray[root].page_latch);
//printf("page unlock [%d]\n",root);
			}

			clearPin(head);
			pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("page unlock [%d]\n",head);
			return table_id;
		}


	}

	
	//printf("open failed\n");
	return FAIL;
}





////////////////////////////// 아래는 다 핀수정만 하면 되지 않을까

//얘는 버퍼뮤텍스 필요x -> pageScan에서 이미 잡고 있음, 대신 페이지락 필요
int pageDrop(int index){
//printf("\npage drop start : victim=%d \n", index);
pthread_mutex_lock(&b_M.frameArray[index].page_latch);
setPin(index);
//printf("page Drop : page lock\n");

	//링크드 리스트에서 삭제
	int pre = b_index.pre;
	int next = b_index.next;

	//drop할 페이지가 헤드였다면
	if (pre == HEAD) {
//printf("pagedrop head \n");
		if (next != TAIL) {
			b_M.LRU_head = b_index.next;
			b_M.frameArray[b_index.next].pre = HEAD;
		}
		else {
			b_M.LRU_head = -1;
		}
	}
	//테일 이었다면
	else if (next == TAIL) {
//printf("pagedrop tail \n");
		b_M.LRU_tail = b_index.pre;
		b_M.frameArray[b_index.pre].next = TAIL;
	}
	//둘다 아닌경우
	else {
//printf("pagedrop normal \n");
		b_M.frameArray[pre].next = next;
		b_M.frameArray[next].pre = pre;
	}

	//초기화
	b_index.pre = -1;
	b_index.next = -1;

	//더티인경우 디스크에 써주고 없애준다
	if (b_index.isdirty) {
//printf("pagedrop dirty \n");
//printf("parent : %ld\n",b_page.parent);
		file_write_page(b_M.table[b_index.table_id - 1].fd, b_index.page_num, &b_page);
	}
	b_index.table_id = 0;
	b_index.page_num = 0;

	//초기화
	b_M.use_num--;
	b_index.isdirty = 0;
	b_index.ispinned = 0;


//printf("page drop end");
clearPin(index);
pthread_mutex_unlock(&b_M.frameArray[index].page_latch);
//printf("page Drop : page unlock[%d]\n",index);
	return SUCCESS;
}

int pageScan(int table, pagenum_t pagenum)
{
//printf("page scan : pagenum = %ld\n",pagenum);
//if(pagenum>500) exit(0);
	pthread_mutex_lock(&buf_latch);
//printf("page scan buf lock\n");

	int victim;
	//원하는 페이지가 버퍼위에 올라가있는지 확인
	//있으면 pick
	//없으면 디스크에서 불러서 올려줘야함
	//불러준후 버퍼매니저 수정

	for (int i = 0; i < b_M.frame_capacity; i++)
	{
		if (b_M.frameArray[i].table_id == table && b_M.frameArray[i].page_num == pagenum) {
//printf("scan_already exist : i=%d\n",i);
			setPin(i);
			pthread_mutex_lock(&b_M.frameArray[i].page_latch);
//printf("scan page lock-1 [%d]\n",i);
			pthread_mutex_unlock(&buf_latch);
//printf("scan buf unlock\n");
			return i;
		}
	}


	//빈자리가 있을경우 버퍼에 올려준다.
	if (b_M.frame_capacity > b_M.use_num) {

		for (int i = 0; i < b_M.frame_capacity; i++)
		{
			//사용중이지 않은 프레임을 찾으면 거기로 페이지를 가져온다
			if (b_M.frameArray[i].pre == -1 && b_M.frameArray[i].next == -1) {
//printf("scan_space exist : pagenum=%ld,i=%d\n",pagenum,i);
				setPage(table, pagenum, i);
				pageLoad(i);
				setPin(i);
				pthread_mutex_lock(&b_M.frameArray[i].page_latch);
//printf("scan page lock-2 [%d]\n",i);
				pthread_mutex_unlock(&buf_latch);
//printf("scan buf unlock\n");
				return i;
			}
		}
	}

	//빈자리가 없는경우
	else {
//printf("scan_space full\n");
			
	

//printf("victim1 : %d\n",b_M.LRU_tail);
		victim = b_M.LRU_tail;
		if (victim == 0) {

			victim = b_M.frameArray[victim].pre;	
			printf("victim change : %d\n",victim);
		}

		//용량이꽉찬경우 drop
		pageDrop(victim);
//printf("page scan : drop complete\n");


	}


//printf("final victim : %d, pagenum : %ld\n",victim,pagenum);
		//비운 프레임에 새 페이지를 읽어온후 정보 갱신
	setPin(victim);
	pthread_mutex_lock(&b_M.frameArray[victim].page_latch);
	setPage(table, pagenum, victim);//read해서 tableid, pagenum 수정
	setDirty(victim);

//printf("page scan : page victim lock\n");
	pageLoad(victim);
//printf("page scan end\n");
	
	pthread_mutex_unlock(&buf_latch);
//printf("page scan : buf unlock\n");
	return victim;
}

//새 페이지를 버퍼에 실었기 때문에 next/pre값을 갱신
int pageLoad(int index)
{
//printf("\npage load : index=%d \n", index);

	b_M.use_num++;

	if (b_M.use_num == 1) {
//printf("page load only use1_return\n");
		b_M.LRU_head = index;
		b_M.LRU_tail = index;
		b_index.pre = HEAD; // 자신이 맨앞
		b_index.next = TAIL; //자신이 맨끝

		return SUCCESS;
	}
//printf("page load more than one..\n");
		//새로 실은 페이지
	b_index.pre = HEAD; // 자신이 맨앞이라는 소리
	b_index.next = b_M.LRU_head;

	//앞뒤로 수정
	b_M.frameArray[b_M.LRU_head].pre = index;
	b_M.LRU_head = index;


//printf("pageload end\n");
	
	return SUCCESS;
}


//모든 프레임이 사용중인지 체크하는 함수
//뮤텍스 할필요 없음
int pinCheck()
{
	for (int i = 1; i < b_M.frame_capacity; i++)
	{

		if (b_M.frameArray[i].ispinned == 0) {
			//printf("pinCheck... : i=%d\n",i);
			return SUCCESS;
		}

	}
	//printf("all page is pinnded. Please wait and try again....\n");
	//실패
	return FAIL;

}


//페이지를 읽어와서 업데이트
//뮤텍스 할필요 없음
void setPage(int table_id, pagenum_t page_num, int index)
{

	
	//bufInfo();
	file_read_page(b_M.table[table_id - 1].fd, page_num, &b_page);
	b_index.table_id = table_id;
	b_index.page_num = page_num;


	//bufInfo();

}

//아래는 전부 set/clear
void setDirty(int index)
{
	b_index.isdirty = 1;
}


void clearDirty(int index)
{
	b_index.isdirty = 0;
}

void setPin(int index) {
	b_index.ispinned = 1;
}

void clearPin(int index) {
	b_index.ispinned = 0;
}

///////////////
//나중에 지울부분
queue * q;
void print_tree(int table_id) {
	printf("\n<print tree>\n");
	pagenum_t nnum, rnum;

	//헤더받아오기
	int head = pageScan(table_id, 0);
	pagenum_t root=b_head.root_page;
	clearPin(head);
	pthread_mutex_unlock(&b_M.frameArray[head].page_latch);

	//if  no root
	if (root == 0) {
		printf("empty tree.\n");
		return;
	}

	q = (queue*)malloc(sizeof(queue));
	q = NULL;

	rnum = root;
	enqueue(rnum);
//printf("print tree 1 : rnum=%ld\n",rnum);
	while (q != NULL) {
//printf("print tree 2\n");
		//dequeue and trace
		nnum = dequeue();
		int tmp = pageScan(table_id, nnum);


		//printf("<my parent = %ld> <my pagenum = %ld> ", b_M.frameArray[tmp].frame_p.parent, nnum);

		for (int i = 0; i < b_M.frameArray[tmp].frame_p.num_key; ++i) {

			printf("%" PRId64 " ", b_M.frameArray[tmp].frame_p.is_leaf ? b_M.frameArray[tmp].frame_p.record[i].key : b_M.frameArray[tmp].frame_p.branch[i].key);
			if(b_M.frameArray[tmp].frame_p.is_leaf)printf(":%s, ", b_M.frameArray[tmp].frame_p.record[i].val);
		}
		//isnot leaf enqueue
		if (!b_M.frameArray[tmp].frame_p.is_leaf) {

			enqueue(b_M.frameArray[tmp].frame_p.right_left);
			//printf("enqueue : %ld\n",b_M.frameArray[tmp].frame_p.right_left);
			for (int i = 0; i < b_M.frameArray[tmp].frame_p.num_key; ++i) {
				enqueue(b_M.frameArray[tmp].frame_p.branch[i].child);
				//printf("enqueue : %ld\n",b_M.frameArray[tmp].frame_p.branch[i].child);
			}
		}
		printf("| ");
		clearPin(tmp);
		pthread_mutex_unlock(&b_M.frameArray[tmp].page_latch);
//printf("print tree unlock suc\n");
	}
	printf("\n");




	


}

void print_tree_direct(int table_id){
printf("\n<print tree direct>\n");

	page_t * n;
	page_t * par_n;
	int i = 0;
	
	pagenum_t nnum, rnum;

	//header get
	header_page * head;
	head = (header_page *)calloc(1, PAGESIZE);
	file_read_page(b_M.table[table_id - 1].fd,0, (page_t *)head);

	//if  no root
	if (head->root_page == 0) {
		printf("empty tree.\n");
		return;
	}

	//printf("header info / pagenum %ld/ free page %ld/ root page %ld\n\n",head->page_num
	//,head->free_page,head->root_page);


	

	n = (page_t *)calloc(1, PAGESIZE);
	par_n = (page_t *)calloc(1, PAGESIZE);
	q = (queue*)malloc(sizeof(queue));
	q = NULL;
	rnum = head->root_page;
	enqueue(rnum);
	while (q != NULL) {
		//dequeue and trace
		nnum = dequeue();

		file_read_page(b_M.table[table_id - 1].fd,nnum, n);
		//printf("<my parent = %ld> <my pagenum = %ld> ",n->parent,nnum);
		for (i = 0; i < n->num_key; ++i) {
	
			printf("%" PRId64 " ", n->is_leaf ? n->record[i].key : n->branch[i].key);
			if(n->is_leaf)printf(":%s, ",n->record[i].val);
		}
		//isnot leaf enqueue
		if (!n->is_leaf) {

			enqueue(n->right_left);
			for (i = 0; i < n->num_key; ++i)
				enqueue(n->branch[i].child);
		}
		printf("| ");
	}
	printf("\n");

	printf( "<free page>\n"); 
	page_t *  freeprint = (page_t *)calloc(1, PAGESIZE);
	file_read_page(b_M.table[table_id - 1].fd,head->free_page, freeprint);

	
	int cnt=0;
	pagenum_t tmp_next=head->free_page;
	if(head->free_page=0) printf("no free page\n");
	else{
	while(1){

		cnt++;
		printf("free %d : %ld \n",cnt,tmp_next);
		tmp_next = freeprint->parent;
		if(freeprint->parent==0)break;
		file_read_page(b_M.table[table_id - 1].fd,freeprint->parent,freeprint);


	}
	}
	
	free(n);
	free(par_n);
	free(q);


}

int showTable()
{
	printf("\n=========<Table List>========\n");

	for (int i = 0; i < 10; i++)
	{
		printf("%d : pathname = %s , table_id = %d , fd = %d, isopen= = %d\n", i + 1, b_M.table[i].path,
			b_M.table[i].id, b_M.table[i].fd, b_M.table[i].isopen);
	}
	printf("=============================\n");

	return SUCCESS;
}



void enqueue(pagenum_t pagenum) {
	queue * enq = (queue *)malloc(sizeof(queue));
	queue * tmp;
	enq->pnum = pagenum;
	enq->next = NULL;

	// q is empty
	if (q == NULL) q = enq;

	// q has page
	else {
		tmp = q;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = enq;
	}
}

pagenum_t dequeue() {
	queue * n = q;
	pagenum_t rt = n->pnum;
	q = q->next;
	free(n);
	return rt;
}

void menu() {
	//menu print funct
	printf("\n\n===please input instruction===\n");
	printf("0.update(u <trx id>  <record id> <val> <table id>)\n1.begin_txn(b)\n2.commit_txn(e <trx_id>)\n3.find(f <table_id> <key> <trx_id>)\n4.print tree(t <table id> <mode>)\n4.quit(q) : auto save \n");
	printf("5.shutdown(s) : close all table\n6.close table(c <table id>)\n7.open new file(o <pathname>)\n8.test(T <test> <mode> - you dont want to use mode, input 0)\n9.show lock list(s <table_id> <key>)\n");
	printf("==============================\n");
}



///////////////
//INSERT

//여기서는 헤더정보 변경x ---alloc을 하면서 프리페이지 수정완료/총개수는 변함x
int make_internal_page(int table_id) {

//printf("\nmake internal page1\n");
	//페이지의 변경할 정보
	//리프/인터널 - numkey, {key, },parent, right_left, isleaf
	//헤더 - 루트,프리,총개수
	int newpage = file_alloc_page(table_id);
//printf("new alloc internal pagenum : %ld\n",b_M.frameArray[newpage].page_num);
	
	//값초기화 
	b_M.frameArray[newpage].frame_p.is_leaf = 0;
	b_M.frameArray[newpage].frame_p.num_key = 0;

	//아직 부모가 결정 안남
	b_M.frameArray[newpage].frame_p.parent = 0;

	//자기밑의 제일 왼쪽 페이지도 결정x
	b_M.frameArray[newpage].frame_p.right_left = 0;

	//키+offset또한 아직 설정할필요x
	setDirty(newpage);
	clearPin(newpage);
	return newpage;
}


int make_leaf_page(int tableid) {
//printf("\nmake leaf page1\n");
	//새 페이지 갖고 옴
	int leaf = make_internal_page(tableid);
//printf("new alloc leaf pagenum : %ld\n",b_M.frameArray[leaf].page_num);

	//리프로 set
	b_M.frameArray[leaf].frame_p.is_leaf = 1;
	setDirty(leaf);
	clearPin(leaf);
	return leaf;
}
/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 삽입할 키 왼쪽에 있는 노드에 대한 부모 포인터의 인덱스를 찾기 위해 insert_into_parent에 사용되는 도우미 기능.
 */

int get_left_index(int tableid,pagenum_t p, pagenum_t l) {

	int i;
	int parent = pageScan(tableid, p);
	int left = pageScan(tableid, l);
pthread_mutex_unlock(&b_M.frameArray[parent].page_latch);
pthread_mutex_unlock(&b_M.frameArray[left].page_latch);

	for (i = 0; i < b_parent.num_key; i++)
	{
		if(b_parent.branch[i].child==l){
			clearPin(parent);
			clearPin(left);

			return i;
		}
	}
	
	clearPin(parent);
	clearPin(left);

	return -1;

}


//////////////////////////////////////////
int insert_into_leaf(int tableid,pagenum_t l, int64_t key, char * val) {
//printf("\ninsert into leaf -- leaf pagenum : %ld\n",l);
	int leaf = pageScan(tableid, l);
pthread_mutex_unlock(&b_M.frameArray[leaf].page_latch);
//printf("insert into leaf : leaf = %d\n",leaf);
	int insertion_point=0;
	
	//삽입위치를 찾는다.
	while (insertion_point < b_leaf.num_key &&  b_leaf.record[insertion_point].key < key)
		insertion_point++;

//printf("insert into leaf : lnsert point = %d\n",insertion_point);
	//삽입위치 자리를 밀어서 비워준다
	for (int i = b_leaf.num_key; i > insertion_point; i--) {
		b_leaf.record[i].key = b_leaf.record[i - 1].key;
		strcpy(b_leaf.record[i].val, b_leaf.record[i - 1].val);
		
	}

	//그 비워둔 위치에 삽입
	b_leaf.record[insertion_point].key = key;
	strcpy(b_leaf.record[insertion_point].val, val);
//printf("insert into leaf---before numkey++ : %d\n",b_leaf.num_key);
	b_leaf.num_key++;
//printf("insert into leaf---after numkey++ : %d\n",b_leaf.num_key);

	//수정된 leaf 다시 write하고 free
	setDirty(leaf);
	clearPin(leaf);
//printf("?...\n");
	return SUCCESS;
}


int insert_into_new_root(int tableid ,pagenum_t l, int64_t key, pagenum_t r) {
	
//printf("\ninsert into new root\n");
	//새 리프페이지를 받아온다
	int root = make_internal_page(tableid);
	int head = pageScan(tableid, 0);
	int right = pageScan(tableid, r);
	int left = pageScan(tableid, l);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
pthread_mutex_unlock(&b_M.frameArray[right].page_latch);
pthread_mutex_unlock(&b_M.frameArray[left].page_latch);


	b_head.root_page = b_M.frameArray[root].page_num;
//printf("\ninsert into new root 2 -- change rootpage=%ld\n",b_head.root_page);
	b_root.parent = 0;//루트 flag
	b_right.parent =b_head.root_page;
	b_left.parent = b_head.root_page;
	
	b_root.num_key++;
	b_root.right_left = l ;
	b_root.branch[0].key = key;
	b_root.branch[0].child = r;
	
	setDirty(root);
	setDirty(head);
	setDirty(right);
	setDirty(left);
	clearPin(root);
	clearPin(head);
	clearPin(right);
	clearPin(left);



	return root;

}


//처음 삽입
int start_new_tree(int tableid,int64_t key,char * val) {

//printf("\nstart new tree 1 \n");

	//새 리프페이지를 받아온다
	int leaf = make_leaf_page(tableid);
//printf("leaf : %d\n",leaf);
	int head = pageScan(tableid, 0);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("start new tree 2 \n");
	//헤더 설정
	b_head.root_page =b_M.frameArray[leaf].page_num;

//printf("start new tree - root : %ld \n",b_head.root_page);
	//b_head.page_num++;

	//리프페이지 설정
	b_leaf.parent = 0;
	b_leaf.num_key = 1;
	b_leaf.record[0].key = key;

	//char *strcpy(char *dest, const char *src);
	//scr를 dest에 복사
	strcpy(b_leaf.record[0].val, val);

	//set and clear
	setDirty(head);
	setDirty(leaf);
	clearPin(leaf);
	clearPin(head);


	return SUCCESS;
}



int db_insert(int tableid,int64_t key, char * value) {
	//printf("db insert 1.....\n");
	//헤더를 받아온다.
	if(b_M.table[tableid-1].isopen==0) {
		printf("File Is Closed\n");
		return FAIL;
	}
//printf("\ndb_insert - 1\n");
	int head = pageScan(tableid, 0);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("db_insert - head find success\n");
	//찾은 경우 -- 이미 있는 경우
//find case--- duplicate
char * str =(char *)malloc(120);
	if (!db_find_2(tableid,key, str)) {
//printf("db_insert - duplicate\n");
		clearPin(head);

//printf("?,duplic\n");
		return FAIL;
	}

	//못찾은경우 -- 넣을 수 있음
	//트리가 안만들어져 있는경우(첫 삽입인 경우)
	if (b_head.root_page == 0) {
//printf("db_insert - start new tree\n");
		clearPin(head);

		return start_new_tree(tableid,key, value);
	}

	//트리가 만들어져있는 경우
	//키가 들어갈 리프페이지 읽어들인다
	int leaf = find_page_2(tableid,key);
	
	pagenum_t l = b_M.frameArray[leaf].page_num;
//printf("db_insert - leaf find success\n");
	//printf("db insert here -- leaf num = %ld\n",leaf);

	//리프 페이지에 남은 자리가 있는 경우
	if (b_leaf.num_key < leaf_order) {
//printf("db_insert - insert into leaf\n");
//printf("num key = %d and leaf order = %d\n",b_leaf.num_key,leaf_order);
clearPin(head);
		clearPin(leaf);
//printf("into leaf before\n");
		insert_into_leaf(tableid,l, key, value);
//printf("leaf after\n");
		return SUCCESS;
	}

	//남은 자리가 없다면 쪼개야함
//printf("db_insert - insert into after splitting\n");
clearPin(head);

	clearPin(leaf);
//printf("before enter\n");
	insert_into_leaf_after_splitting(tableid,l, key, value);
//printf("after enter\n");
	
	return SUCCESS;

	//그 키가 이미 있는지 확인한다.
	//없다면 새 레코드를 생성한다
	//만약 루트노드도 없다면 그 키로 루트를 새로 만들어준다
	//그 키가 들어갈 페이지를 가져온다
	//insert into leaf시전
	//리턴으로 leaf after split호출

}



int insert_into_leaf_after_splitting(int tableid,pagenum_t l, int64_t key, char * val) {
//printf("\ninsert into leaf after splitting 1\n");
	//원래 있던 리프 만들어주기
	int leaf = pageScan(tableid, l);
	int insertion_index, split, new_key, i, j;

	//임시공간 마련
	key_val* tmp_record = (key_val *)malloc(33*sizeof(struct record));

	//쪼개서 넣어줄 새 리프 만들어주기
	int new_p = make_leaf_page(tableid);
	pagenum_t n_p = b_M.frameArray[new_p].page_num;
//printf("insert into leaf after splitting 2 --- leaf pagenum = %ld\n",n_p);
	//tmp키포인터와 포인터를 가리키는 포인터를 만들어둔다 ? 왜 ---
	//그니까 원래3개만 들어갈수 있는데 하나가 더들어온경우 4개짜리를 보관할수있는 temp를 만들고
	//그걸 split해서 두개로 쪼개서 넣어준다는 말씀

pthread_mutex_unlock(&b_M.frameArray[leaf].page_latch);
pthread_mutex_unlock(&b_M.frameArray[new_p].page_latch);

	//삽입위치 찾기
	insertion_index = 0;
	while (insertion_index < leaf_order && b_leaf.record[insertion_index].key < key)
		insertion_index++;

//printf("insert into leaf after splitting 3 -- insert index = %d\n",insertion_index);
	//삽입위치인경우 하나띄워서 leat의 값을 복사해서 가져와서 저장 
	for (i = 0, j = 0; i < b_leaf.num_key; i++, j++) {
		if (j == insertion_index) j++;
		tmp_record[j].key = b_leaf.record[i].key;
		strcpy(tmp_record[j].val, b_leaf.record[i].val);
	}
	//빈 삽입위치에 넣어주기
	tmp_record[insertion_index].key = key;
	strcpy(tmp_record[insertion_index].val, val);


	//원래 리프를 비우고 다시 채워준다
	b_leaf. num_key= 0;

	//분할지점 정하기
	split = cut(leaf_order);
//printf("insert into leaf after splitting 4 -- split=%d\n",split);
	//원래 리프의 쪼갠부분의 앞쪽을 넣어주기
	for (i = 0; i < split; i++) {
		strcpy(b_leaf.record[i].val , tmp_record[i].val);
		b_leaf.record[i].key = tmp_record[i].key;
		b_leaf.num_key++;
	}
	//새 리프에 쪼갠부분의 나머지 부분을 넣어주기
	for (i = split, j = 0; i <= leaf_order; i++, j++) {
		strcpy(b_M.frameArray[new_p].frame_p.record[j].val , tmp_record[i].val);
		b_new.record[j].key = tmp_record[i].key;
		b_new.num_key++;
	}

	//tmp공간 free
	

	//새 리프랑 원래리프를 연결시켜준다  물론 그전에 원래리프가 가리키고 있던
	//다음 부분을 새로만든게 가리킬수 있도록 업뎃을 먼저 해둬야함
	b_new.right_left = b_leaf.right_left;
	b_leaf.right_left = n_p;

	//원래 리프를 재구성했으므로 원래값이 들어있던 부분들이 사라졌기때문에
	//거기에 대한 조정을 해준다
	for (i = b_leaf.num_key; i < leaf_order ; i++) {
		b_leaf.record[i].key = -1;
		
	}


	//값이 들은곳 뒤쪽부분 정리
	for (i = b_new.num_key; i < leaf_order ; i++) {
		b_new.record[i].key = -1;
		
	}

	//새로 만들어준 리프를 부모와 연결시켜주고 new키 설정 
	b_new.parent = b_leaf.parent;
	new_key = b_new.record[0].key;

	setDirty(leaf);
	setDirty(new_p);
	clearPin(leaf);
	clearPin(new_p);

	free(tmp_record);
//printf("befor in to parent\n");
	return insert_into_parent(tableid,l, new_key, n_p);
}


int insert_into_node(int tableid,pagenum_t n_p,int left_index, int64_t key, pagenum_t right) {
//printf("\ninsert into node 1 --- left index = %d\n",left_index);	
	int i;
	//n_p에 parent를 받아옴
	int page_parent = pageScan(tableid, n_p);
pthread_mutex_unlock(&b_M.frameArray[page_parent].page_latch);
	//left 인덱스까지 한칸씩 오른쪽으로 미뤄서 자리를 확보한다
	for (i = b_page_parent.num_key; i > left_index; i--) {
		b_page_parent.branch[i + 1].child = b_page_parent.branch[i].child;
		b_page_parent.branch[i + 1].key = b_page_parent.branch[i].key;
	}

	//키와 포인터를 삽입
	b_page_parent.branch[left_index + 1].child = right;
	b_page_parent.branch[left_index+1].key = key;
	b_page_parent.num_key++;
	
	setDirty(page_parent);
	clearPin(page_parent);


//printf("insert into node end\n");
	return SUCCESS;
}

int insert_into_node_after_splitting(int tableid,pagenum_t old, int left_index, int64_t key, pagenum_t right) {

//printf("\ninsert into node after splitting 1\n");
	//into parent함수에서 old에 parent를 넘겨줌
	int i, j, split;
	int64_t k_prime;
	
	//원래 있던 노드 만들어주기
	int old_p = pageScan(tableid, old);

	//쪼개서 넣어줄 새 노드 만들어주기
	int new_p = make_internal_page(tableid);
	pagenum_t n_p = b_M.frameArray[new_p].page_num;

pthread_mutex_unlock(&b_M.frameArray[old_p].page_latch);
pthread_mutex_unlock(&b_M.frameArray[new_p].page_latch);
 //printf("\ninsert into node after splitting 2 -- alloc complete : %d \n",new_p);
	/*
	 먼저 새 키와 포인터를 포함한 모든 것을 올바른 위치에
	 삽입할 수 있는 임시 키와 포인터를 만드십시오.
	 그런 다음 새 노드를 생성하고 키와 포인터의 절반은 이전 노드에,
	 나머지 절반은 새 노드에 복사하십시오.
	 */
	 //임시공간 마련
	key_child * tmp_key_child = (key_child *)malloc(250 * sizeof(struct key_child));

	//삽입할 위치를 비워준다
	for (i = 0, j = 0; i < b_old.num_key + 1; i++, j++) {
		if (j == left_index + 1) j++;
		tmp_key_child[j].child = b_old.branch[i].child;
	}
	for (i = 0, j = 0; i < b_old.num_key; i++, j++) {
		if (j == left_index+1) j++;
		tmp_key_child[j].key = b_old.branch[i].key;
	}

	//삽입할 위치에 삽입
	tmp_key_child[left_index + 1].child = right;
	tmp_key_child[left_index+1].key = key;

	/* Create the new node and copy
	 * half the keys and pointers to the
	 * old and half to the new.
	 새 노드를 생성하고 키와 포인터의 절반을 기존 노드에, 포인터 절반은 새 노드에 복사하십시오.
	 */
	 //쪼개는 위치 찾기
	split = cut(node_order);
	//원래 노드 비워주기
	b_old.num_key = 0;

	//원래있던 노드 채우기
	for (i = 0; i < split-1; i++) {
		b_old.num_key++;
		b_old.branch[i].key = tmp_key_child[i].key;
		b_old.branch[i].child= tmp_key_child[i].child;
		
	}

	//리프의 경우는 자신이 자식이 없기 때문에 그냥 올리는 작업을 했을때 문제x
	//그러나 인터널의 경우 자식을 갖고 올라가게 되므로
	//자신의 자식정보 수정이 필요


	k_prime = tmp_key_child[split-1].key;//올라갈 친구의 k_primed에 저장

	
	//새로만든 노드채우기
	b_new.num_key = 0;
	//split 지점 부터 넣음
	for (++i, j = 0; i <= node_order; i++, j++) {
		b_new.num_key++;
		b_new.branch[j].key = tmp_key_child[i].key;
		b_new.branch[j].child = tmp_key_child[i].child;
		
	}
	
	//올린 키가 원래 가리키고 있던 자식을 새로만든 노드의 right_left(rightmost)가 가리키게 설정
	b_new.right_left= tmp_key_child[split-1].child;

	//새로만든 노드 부모설정해주기
	b_new.parent = b_old.parent;

	//새로만든 노드의 자식의 부모설정도 마무리
	int child;
	for (i = 0; i < b_new.num_key; i++) {
//printf("\nchild changing...\n");
		child = pageScan(tableid, b_new.branch[i].child);
		b_child.parent = n_p;
		setDirty(child);
		clearPin(child);
pthread_mutex_unlock(&b_M.frameArray[child].page_latch);
		
	}
//printf("\nchild changing...\n");
	child = pageScan(tableid,b_new.right_left);
	b_child.parent =n_p;
	setDirty(child);

	setDirty(old_p);
	setDirty(new_p);
	clearPin(old_p);
	clearPin(new_p);
	clearPin(child);


pthread_mutex_unlock(&b_M.frameArray[child].page_latch);

	return insert_into_parent(tableid,old, k_prime, n_p);//올라갈 친구를 선정해서 부모에 삽입
}


int insert_into_parent(int tableid,pagenum_t l, int64_t key, pagenum_t right) {
//printf("\ninsert into parent 1\n");
	int left_index;
	pagenum_t p_page;

	int left = pageScan(tableid, l);
	p_page = b_left.parent;
pthread_mutex_unlock(&b_M.frameArray[left].page_latch);

//printf("insert into parent 2 : left=%d,parent page=%ld\n",left,p_page);

	//새루트에 넣어야하는 경우(즉 left가 루트(internal)였던 경우)
	if (p_page == 0) {
//printf("insert into new root call\n");
		clearPin(left);
		return insert_into_new_root(tableid,l, key, right);
	}

	//아닌경우 부모를 받아온다
	int parent = pageScan(tableid, p_page);
	pthread_mutex_unlock(&b_M.frameArray[parent].page_latch);
	//left가 부모쪽에서 몇번째 인덱스에 있는지 확인
//printf("insert into parent : parent = %ld\n",p_page);
	left_index = get_left_index(tableid,p_page, l);

	//부모페이지에 올려준다
	if (b_parent.num_key < node_order) {
//printf("insert into node call\n");
		clearPin(left);
		clearPin(parent);

		return insert_into_node(tableid,p_page, left_index, key, right);
	}
	 //부모로 하나 올렸는데 거기서도 꽉찬경우 다시 쪼개줘야함
	clearPin(left);
	clearPin(parent);


//printf("insert into node splitting call\n");
	return insert_into_node_after_splitting(tableid,p_page, left_index, key, right);
}



int cut(int length) {
	if (length % 2 == 0)
		return length / 2;
	else
		return length / 2 + 1;
}


int db_find_2(int tableid,int64_t key, char * ret_val) {
	int i;
	if(b_M.table[tableid-1].isopen==0) {
		printf("File Is Closed\n");
		return FAIL;
	}
//printf("\ndb_find-1\n");
	//그 키가 들어있을수도 있는 리프페이지를 가져온다
	int find_p = find_page_2(tableid,key);

//printf("db_find --- find page success, find_p = %d\n",find_p);
	//빈트리인 경우
	if (find_p == FAIL) {
//printf("db_find- empty\n");
		//clearPin(find_p);
		return FAIL;
	}
//printf("?..\n");

	//키를 발견하면 멈춘다
	for (i = 0; i < b_M.frameArray[find_p].frame_p.num_key; i++)
	{
//printf("[%d]check... ",i);
		if (b_M.frameArray[find_p].frame_p.record[i].key == key) break;
	}
//printf("now i = %d and num_key =%d\n",i,b_M.frameArray[find_p].frame_p.num_key);
	//인덱스가 끝번이 아니라면 찾지못한것 - break로 멈춰진게 아니라는 뜻
	if (i == b_M.frameArray[find_p].frame_p.num_key) {
//printf("db_find----- not find\n");
		clearPin(find_p);
		return FAIL;
	}
	else {
//printf("db_find------ find\n");
		strcpy(ret_val, b_M.frameArray[find_p].frame_p.record[i].val);
//printf("???\n");
		clearPin(find_p);
//printf("?..\n");
		return SUCCESS;
	}
	//만약 받아온 페이지가 null이라면 리턴종료
	//있다면 그 페이지 내에서 key와 일치하는 것을 찾는다
	//for 돌린후 키의 인덱스를 찾는다 i 5 
	//그 인덱스가 numkey와 같다면 못찾은것 --- -1리턴
	//아니라면 val를 저장하고  0이런
}



int find_page_2(int tableid,int64_t key) {

//printf("\nfind page 1\n");
	int i = 0;
	//헤더를 받아온다
	int head = pageScan(tableid, 0);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("0");
	//빈트리 일경우
	if (b_head.root_page == 0) {
//printf("find page - empty\n");
		clearPin(head);


		return FAIL;
	}

	//루트정보저장
	int trace = pageScan(tableid, b_head.root_page);
	pagenum_t tmp=b_head.root_page;

//printf("1\n");
	while (!b_M.frameArray[trace].frame_p.is_leaf) {
		i=0;
//printf("2\n");
		while (i < b_M.frameArray[trace].frame_p.num_key) {
			if (key >= b_M.frameArray[trace].frame_p.branch[i].key) {
				if(i==(b_M.frameArray[trace].frame_p.num_key -1)){
				break;
				}
				i++;
			}
			else {
				i--;
				break;
			}

		}

		if (i != -1) {
//printf("find page - 2\n");
			tmp = b_M.frameArray[trace].frame_p.branch[i].child;
clearPin(trace);
pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
//printf("\nunlock page [%d]\n",trace);
			trace = pageScan(tableid, tmp);
//printf("lock new  page [%d]\n",trace);
		}
		else if (i == -1) {
//printf("find page - 3\n");
			tmp = b_M.frameArray[trace].frame_p.right_left;
clearPin(trace);
pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
			//printf("unlock page [%d]\n",trace);
			trace = pageScan(tableid, tmp);
//printf("lock new  page [%d]\n",trace);
		}
		
	
	}
	//while 이 끝났다는건 리프 까지 도착
	//이때 반환되는 리프 페이지에는 실제 키가 들어있을 수도 있고 없을 수도 있다
	//예를 들어 9를 찾는 경우, 7이상 10미만의 키들이 들어있어야하는 페이지가 반환되어도
	//거기에 9가 있다는 보장은 x
pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
	int find = pageScan(tableid, tmp);
//printf("find page - 4\n");

	clearPin(trace);

	clearPin(find);
pthread_mutex_unlock(&b_M.frameArray[find].page_latch);
	return find;
}


