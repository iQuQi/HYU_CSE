
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<inttypes.h>//PRid64 - int64_t�� ������ ���
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





//���⼭ ���۶� �������� ���۾�� ����
int find_page(int tableid, int64_t key,int trx_id,int mode) {

//printf("\nfind page[%d] 1\n",trx_id);
	int i = 0;
	//����� �޾ƿ´� -> �� �ȿ��� ���۶� -> ������ã�� �������� -> ���۾��
	int head = pageScan(tableid, 0);
	pagenum_t rootp=b_head.root_page;
	clearPin(head);
	pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
	//printf("find page[%d] : page unlock head [%d] \n", trx_id, head );


	//��Ʈ�� �ϰ��
	if (rootp == 0) {
//printf("find page[%d] - empty\n", trx_id);
		return FAIL;
	}

	//trx ���̺��� Ȯ��
	if (!trx_find(trx_id)) {
		//printf("find page[%d]: not exist trx\n", trx_id);
		return FAIL;
	}

	//��Ʈ��������
	int trace = pageScan(tableid, rootp);
	lock_t * tmp_l;
//printf("find page[%d] : trace page lock\n", trx_id);
	pagenum_t tmp = rootp;

	//����� �����൵ �ɵ� 
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
	//����� �޾ƿ´� -> �� �ȿ��� ���۶� -> ������ã�� �������� -> ���۾��
	int find = pageScan(tableid, tmp);
//printf("find page[%d] - 4\n", trx_id);

	clearPin(find);
	pthread_mutex_unlock(&b_M.frameArray[find].page_latch);
	//printf("find page[%d] : page unlock3 [%d] \n", trx_id,find);
	//���� ��������� 3��

	//������������ ��� mutex���� ����

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

	//trx ���̺��� Ȯ��
	if (!trx_find(trx_id)) {
	//	printf("db find[%d] : not exist trx\n", trx_id);
		return SUCCESS;
	}


//printf("\ndb_find[%d] start -1\n",trx_id);
		//�� Ű�� ����������� �ִ� ������������ �����´�
	
	int find_p = find_page(table_id, key,trx_id,0);
	lock_t * tmp_l;
	//���۶� 
	
//printf("db find page lock [%d] suc\n",find_p);
	//���۾��
	
	//��Ʈ���� ���
	if (find_p == FAIL) {
//printf("db_find[%d]- empty\n",trx_id);
		
//printf("db find[%d] : page unlock-fail [%d] \n",trx_id,find_p);
		return SUCCESS;
	}

	setPin(find_p);
	pthread_mutex_lock(&b_M.frameArray[find_p].page_latch);

	//���������� ������ ��
	//Ű�� �߰��ϸ� ����� = ���������->���ڵ�� ->��������->�б�->���ڵ���(��� ��) --- ���� ���ڵ�� �̰� �ݺ�(abort���� Ȯ��),
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
		//�ε����� ������ �ƴ϶�� ã�����Ѱ� - break�� �������� �ƴ϶�� ��
	if (i == b_M.frameArray[find_p].frame_p.num_key) {
//printf("db_find[%d] end----- not find\n",trx_id);
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("db find[%d] : page unlock-1 [%d] \n",trx_id,find_p);
		return SUCCESS;
	}
	else {
//printf("db_find end[%d]------ find\n",trx_id);
//�̰������� ���ڵ�� �۾� S���� (abortȮ��) -> ���ڵ� ��� ����-> commit�ÿ� ��
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


//Ű�� �ش��ϴ� �������� �о�ͼ� ���ڵ� ���� 
//������ 0���� 
//���н� nonzero -> abort �ʿ� -> ���� ���� release ���ϰ� undo
//->E ���, �տ� ���� ��� �ִٸ� ������ ��ٸ�
//Abort���ٸ� ABORT����
int db_update(int table_id, int64_t key, char * val, int trx_id)
{

	//if(b_M.frameArray==NULL) init_db(500);
	int i;
	char * str_tmp= (char *)malloc(120);
	//printf("\ndb update start[%d]\n",trx_id);
	//find�� �ؼ� �ش� �������� �о��-E����
	//������ �����ٴ°� ������� �Ͼ�� ������ �ǹ��ϰ� ���� �� lock�� wakeup�� ���¶�� ����
	//->���Լ� ���������� �����Ұ� ���������
	if (b_M.table[table_id - 1].isopen == 0) {
		printf("File Is Closed\n");
		return SUCCESS;
	}

	//trx ���̺��� Ȯ��
	if (!trx_find(trx_id)) {
		//printf("db update : not exist trx\n");
		return SUCCESS;
	}
	
//printf("db update[%d] 1\n",trx_id);
	int f_c = db_find(table_id, key, str_tmp, trx_id);
	
	if (f_c == ABORT) {return ABORT;}

//printf("db update[%d] 2\n",trx_id);
	int find_p = find_page(table_id, key,trx_id,0);
	
	//���۶� �������� ���۾��
	
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

		//�ε����� ������ �ƴ϶�� ã�����Ѱ� - break�� �������� �ƴ϶�� ��
	if (i == b_M.frameArray[find_p].frame_p.num_key) {
		clearPin(find_p);
		pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
		//printf("not find the date\n")
		//printf("db find[%d] : page unlock-1 [%d] \n",trx_id,find_p);
		return SUCCESS;
	}



//printf("db_update[%d]: find end------ find, pre val=%s\n",trx_id,b_M.frameArray[find_p].frame_p.record[i].val);
//�̰������� ���ڵ�� �۾� S���� (abortȮ��) -> ���ڵ� ��� ����-> commit�ÿ� ��
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

		//������Ʈ �α� �߻�
	//int log_update(int trx_id,int table_id,int64_t page_num,char* old_image,char* new_image);
		int LSN=log_update(trx_id, table_id, find_p, old, val, i);
		b_M.frameArray[find_p].frame_p.page_LSN = LSN;
		

	//lock������ ���� commit ������ ��ٸ�
	//printf("db update[%d] SUCCESS end, change val=%s\n",trx_id,val);
	clearPin(find_p);
	pthread_mutex_unlock(&b_M.frameArray[find_p].page_latch);
//printf("dp update[%d] : mutex unlock [%d] FINISH \n",trx_id,find_p);


	
	
	
	return SUCCESS;
}




//5������ �Ʒ� �Լ� �� �����ϱ�

//�Լ� ������ 0���� ���н� -1 ����
int ch;

//���� : page lock ���� Ȯ���� �� �ְ�
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


//pin ���� ���� ����
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

	//���������
}


//�� ���� ����
void headInfo(int tableid) {
	//if(b_M.frameArray==NULL) init_db(500);
	int head = pageScan(tableid, 0);
	
	printf("\n<header info>\n");
	printf("root , free, pagenum = %ld, %ld, %ld\n", b_head.root_page, b_head.free_page, b_head.page_num);
	clearPin(head);
	pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
printf("page unlock [%d]\n",head);

}

//�� ���� ����
int close_table(int table_id) {
//printf("\nclosetable1 -- frame capacity : %d\n",b_M.frame_capacity);
	if (table_id > 10 || table_id < 1) {
//printf("wrong close\n");
		return FAIL;
	}

	//�� ���̺��� ���������� ���� ��ũ�� �����ش�.
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

	//���̺� �迭���� �����ش�
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

//���� x
int shutdown_db()
{
	//���̺� �ö���ִ� ������ŭ close table
	for (int i = 0; i < 10; i++)
	{
		close_table(b_M.table[i].id);
	}

	return SUCCESS;
}

////////////////////////////////////////////


//����x
int init_db(int buf_num,int flag, int log_num,char* log_path, char* logmsg_path)
{
//printf("\ninit db 1\n");
		//�����Ҵ� �����ش�, calloc�� ��� 0�� �ʱ�ȭ... �׷��� Ȥ�� �𸣴Ϥ�
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

	if (flag == 0) {//�븻
		//�α� ���Ͽ� �ִ� ��� �α׷��ڵ带 ����
		//�м�->redo->undo�� ��� �����ϵ��� �Ѵ�
		
		//�м�
		log_M.log_now=analysis();//�α������� ���鼭 ������ ã�´�.

		//redo
		recovery_redo(0);//��� �α׸� �����

		//undo
		recovery_undo(0);//������ ���ؼ� undo ����

	}
	else if (flag == 1) {//redo crash
		//redo �ܰ迡�� �־��� �α� seq num���� ��� �����ϰ� 

		//�м�
		log_M.log_now=analysis();

		//redo
		recovery_redo(log_num);
	}
	else if (flag == 2) {//undo crash
		//undo�ܰ迡�� �־���  �α� seq num���� ���� �����ϰ� 

		//�м�
		log_M.log_now=analysis();

		//redo
		recovery_redo(0);

		//undo
		recovery_undo(log_num);
	}

	//�α� ���ۿ� ������ ������ dirty�� ���� ��ũ�� �����ش�

	return SUCCESS;
}


//�� ����
int open_table(char *pathname) {
	
	//if(b_M.frameArray==NULL) init_db(500);
	//����� �о�´�.
	int root;
	int head;
	//O_creat ��忡���� ����° ���ڿ� ���Ͽ� ���ٱ��� ��������
	//������ ���� ��� ���� �������ִµ� ������ �����Ѵٸ� ������ ����(O_EXCL)


	//���� ���ϸ��� �����ϴ��� üũ
	int i;
	for (i = 0; i < 10; i++)
	{
		if (!strcmp(pathname, b_M.table[i].path)) {
			break;
		}
	}

//printf("open table : i= %d\n",i);
		//������ �������� ���� ���̺�
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
		//���������� ���ȴٸ�
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
			//��� ������ ���� ī��Ʈ
			b_head.page_num = 1;
			b_head.free_page = 0;
			b_head.root_page = 0;

			//��� dirty/pin set
			clearPin(head);
			pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("page unlock [%d]\n",head);
			setDirty(head);

			return table_id;
		}

		//�����Ǿ��ִٸ� �ٽ� �о�´�.
		b_M.table[i].fd = open(pathname, O_RDWR);
		table_fd = b_M.table[i].fd;

		//�������� �о������
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
						//��Ʈ�������� ���ۿ� �÷��ش�
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

	//������ �������� �־� ������ ����Ǿ��ִ� ��� fd�� �����ؼ� ����
	else {
//printf("open table : already open-1\n");
		b_M.table[i].fd = open(pathname, O_RDWR);
//printf("new fd : %d\n",b_M.table[i].fd);
		int table_fd = b_M.table[i].fd;
		int table_id = b_M.table[i].id;

		if (table_fd > 0) {
//printf("open table : already open-2\n");
						//����� ���ۿ� �÷��ش�
			head = pageScan(table_id, 0);
			b_M.table[i].isopen = 1;
			b_M.table_use++;

			//��Ʈ�������� ���ۿ� �÷��ش�
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





////////////////////////////// �Ʒ��� �� �ɼ����� �ϸ� ���� ������

//��� ���۹��ؽ� �ʿ�x -> pageScan���� �̹� ��� ����, ��� �������� �ʿ�
int pageDrop(int index){
//printf("\npage drop start : victim=%d \n", index);
pthread_mutex_lock(&b_M.frameArray[index].page_latch);
setPin(index);
//printf("page Drop : page lock\n");

	//��ũ�� ����Ʈ���� ����
	int pre = b_index.pre;
	int next = b_index.next;

	//drop�� �������� ��忴�ٸ�
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
	//���� �̾��ٸ�
	else if (next == TAIL) {
//printf("pagedrop tail \n");
		b_M.LRU_tail = b_index.pre;
		b_M.frameArray[b_index.pre].next = TAIL;
	}
	//�Ѵ� �ƴѰ��
	else {
//printf("pagedrop normal \n");
		b_M.frameArray[pre].next = next;
		b_M.frameArray[next].pre = pre;
	}

	//�ʱ�ȭ
	b_index.pre = -1;
	b_index.next = -1;

	//��Ƽ�ΰ�� ��ũ�� ���ְ� �����ش�
	if (b_index.isdirty) {
//printf("pagedrop dirty \n");
//printf("parent : %ld\n",b_page.parent);
		file_write_page(b_M.table[b_index.table_id - 1].fd, b_index.page_num, &b_page);
	}
	b_index.table_id = 0;
	b_index.page_num = 0;

	//�ʱ�ȭ
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
	//���ϴ� �������� �������� �ö��ִ��� Ȯ��
	//������ pick
	//������ ��ũ���� �ҷ��� �÷������
	//�ҷ����� ���۸Ŵ��� ����

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


	//���ڸ��� ������� ���ۿ� �÷��ش�.
	if (b_M.frame_capacity > b_M.use_num) {

		for (int i = 0; i < b_M.frame_capacity; i++)
		{
			//��������� ���� �������� ã���� �ű�� �������� �����´�
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

	//���ڸ��� ���°��
	else {
//printf("scan_space full\n");
			
	

//printf("victim1 : %d\n",b_M.LRU_tail);
		victim = b_M.LRU_tail;
		if (victim == 0) {

			victim = b_M.frameArray[victim].pre;	
			printf("victim change : %d\n",victim);
		}

		//�뷮�̲������ drop
		pageDrop(victim);
//printf("page scan : drop complete\n");


	}


//printf("final victim : %d, pagenum : %ld\n",victim,pagenum);
		//��� �����ӿ� �� �������� �о���� ���� ����
	setPin(victim);
	pthread_mutex_lock(&b_M.frameArray[victim].page_latch);
	setPage(table, pagenum, victim);//read�ؼ� tableid, pagenum ����
	setDirty(victim);

//printf("page scan : page victim lock\n");
	pageLoad(victim);
//printf("page scan end\n");
	
	pthread_mutex_unlock(&buf_latch);
//printf("page scan : buf unlock\n");
	return victim;
}

//�� �������� ���ۿ� �Ǿ��� ������ next/pre���� ����
int pageLoad(int index)
{
//printf("\npage load : index=%d \n", index);

	b_M.use_num++;

	if (b_M.use_num == 1) {
//printf("page load only use1_return\n");
		b_M.LRU_head = index;
		b_M.LRU_tail = index;
		b_index.pre = HEAD; // �ڽ��� �Ǿ�
		b_index.next = TAIL; //�ڽ��� �ǳ�

		return SUCCESS;
	}
//printf("page load more than one..\n");
		//���� ���� ������
	b_index.pre = HEAD; // �ڽ��� �Ǿ��̶�� �Ҹ�
	b_index.next = b_M.LRU_head;

	//�յڷ� ����
	b_M.frameArray[b_M.LRU_head].pre = index;
	b_M.LRU_head = index;


//printf("pageload end\n");
	
	return SUCCESS;
}


//��� �������� ��������� üũ�ϴ� �Լ�
//���ؽ� ���ʿ� ����
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
	//����
	return FAIL;

}


//�������� �о�ͼ� ������Ʈ
//���ؽ� ���ʿ� ����
void setPage(int table_id, pagenum_t page_num, int index)
{

	
	//bufInfo();
	file_read_page(b_M.table[table_id - 1].fd, page_num, &b_page);
	b_index.table_id = table_id;
	b_index.page_num = page_num;


	//bufInfo();

}

//�Ʒ��� ���� set/clear
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
//���߿� ����κ�
queue * q;
void print_tree(int table_id) {
	printf("\n<print tree>\n");
	pagenum_t nnum, rnum;

	//����޾ƿ���
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

//���⼭�� ������� ����x ---alloc�� �ϸ鼭 ���������� �����Ϸ�/�Ѱ����� ����x
int make_internal_page(int table_id) {

//printf("\nmake internal page1\n");
	//�������� ������ ����
	//����/���ͳ� - numkey, {key, },parent, right_left, isleaf
	//��� - ��Ʈ,����,�Ѱ���
	int newpage = file_alloc_page(table_id);
//printf("new alloc internal pagenum : %ld\n",b_M.frameArray[newpage].page_num);
	
	//���ʱ�ȭ 
	b_M.frameArray[newpage].frame_p.is_leaf = 0;
	b_M.frameArray[newpage].frame_p.num_key = 0;

	//���� �θ� ���� �ȳ�
	b_M.frameArray[newpage].frame_p.parent = 0;

	//�ڱ���� ���� ���� �������� ����x
	b_M.frameArray[newpage].frame_p.right_left = 0;

	//Ű+offset���� ���� �������ʿ�x
	setDirty(newpage);
	clearPin(newpage);
	return newpage;
}


int make_leaf_page(int tableid) {
//printf("\nmake leaf page1\n");
	//�� ������ ���� ��
	int leaf = make_internal_page(tableid);
//printf("new alloc leaf pagenum : %ld\n",b_M.frameArray[leaf].page_num);

	//������ set
	b_M.frameArray[leaf].frame_p.is_leaf = 1;
	setDirty(leaf);
	clearPin(leaf);
	return leaf;
}
/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 ������ Ű ���ʿ� �ִ� ��忡 ���� �θ� �������� �ε����� ã�� ���� insert_into_parent�� ���Ǵ� ����� ���.
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
	
	//������ġ�� ã�´�.
	while (insertion_point < b_leaf.num_key &&  b_leaf.record[insertion_point].key < key)
		insertion_point++;

//printf("insert into leaf : lnsert point = %d\n",insertion_point);
	//������ġ �ڸ��� �о ����ش�
	for (int i = b_leaf.num_key; i > insertion_point; i--) {
		b_leaf.record[i].key = b_leaf.record[i - 1].key;
		strcpy(b_leaf.record[i].val, b_leaf.record[i - 1].val);
		
	}

	//�� ����� ��ġ�� ����
	b_leaf.record[insertion_point].key = key;
	strcpy(b_leaf.record[insertion_point].val, val);
//printf("insert into leaf---before numkey++ : %d\n",b_leaf.num_key);
	b_leaf.num_key++;
//printf("insert into leaf---after numkey++ : %d\n",b_leaf.num_key);

	//������ leaf �ٽ� write�ϰ� free
	setDirty(leaf);
	clearPin(leaf);
//printf("?...\n");
	return SUCCESS;
}


int insert_into_new_root(int tableid ,pagenum_t l, int64_t key, pagenum_t r) {
	
//printf("\ninsert into new root\n");
	//�� ������������ �޾ƿ´�
	int root = make_internal_page(tableid);
	int head = pageScan(tableid, 0);
	int right = pageScan(tableid, r);
	int left = pageScan(tableid, l);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
pthread_mutex_unlock(&b_M.frameArray[right].page_latch);
pthread_mutex_unlock(&b_M.frameArray[left].page_latch);


	b_head.root_page = b_M.frameArray[root].page_num;
//printf("\ninsert into new root 2 -- change rootpage=%ld\n",b_head.root_page);
	b_root.parent = 0;//��Ʈ flag
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


//ó�� ����
int start_new_tree(int tableid,int64_t key,char * val) {

//printf("\nstart new tree 1 \n");

	//�� ������������ �޾ƿ´�
	int leaf = make_leaf_page(tableid);
//printf("leaf : %d\n",leaf);
	int head = pageScan(tableid, 0);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("start new tree 2 \n");
	//��� ����
	b_head.root_page =b_M.frameArray[leaf].page_num;

//printf("start new tree - root : %ld \n",b_head.root_page);
	//b_head.page_num++;

	//���������� ����
	b_leaf.parent = 0;
	b_leaf.num_key = 1;
	b_leaf.record[0].key = key;

	//char *strcpy(char *dest, const char *src);
	//scr�� dest�� ����
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
	//����� �޾ƿ´�.
	if(b_M.table[tableid-1].isopen==0) {
		printf("File Is Closed\n");
		return FAIL;
	}
//printf("\ndb_insert - 1\n");
	int head = pageScan(tableid, 0);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("db_insert - head find success\n");
	//ã�� ��� -- �̹� �ִ� ���
//find case--- duplicate
char * str =(char *)malloc(120);
	if (!db_find_2(tableid,key, str)) {
//printf("db_insert - duplicate\n");
		clearPin(head);

//printf("?,duplic\n");
		return FAIL;
	}

	//��ã����� -- ���� �� ����
	//Ʈ���� �ȸ������ �ִ°��(ù ������ ���)
	if (b_head.root_page == 0) {
//printf("db_insert - start new tree\n");
		clearPin(head);

		return start_new_tree(tableid,key, value);
	}

	//Ʈ���� ��������ִ� ���
	//Ű�� �� ���������� �о���δ�
	int leaf = find_page_2(tableid,key);
	
	pagenum_t l = b_M.frameArray[leaf].page_num;
//printf("db_insert - leaf find success\n");
	//printf("db insert here -- leaf num = %ld\n",leaf);

	//���� �������� ���� �ڸ��� �ִ� ���
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

	//���� �ڸ��� ���ٸ� �ɰ�����
//printf("db_insert - insert into after splitting\n");
clearPin(head);

	clearPin(leaf);
//printf("before enter\n");
	insert_into_leaf_after_splitting(tableid,l, key, value);
//printf("after enter\n");
	
	return SUCCESS;

	//�� Ű�� �̹� �ִ��� Ȯ���Ѵ�.
	//���ٸ� �� ���ڵ带 �����Ѵ�
	//���� ��Ʈ��嵵 ���ٸ� �� Ű�� ��Ʈ�� ���� ������ش�
	//�� Ű�� �� �������� �����´�
	//insert into leaf����
	//�������� leaf after splitȣ��

}



int insert_into_leaf_after_splitting(int tableid,pagenum_t l, int64_t key, char * val) {
//printf("\ninsert into leaf after splitting 1\n");
	//���� �ִ� ���� ������ֱ�
	int leaf = pageScan(tableid, l);
	int insertion_index, split, new_key, i, j;

	//�ӽð��� ����
	key_val* tmp_record = (key_val *)malloc(33*sizeof(struct record));

	//�ɰ��� �־��� �� ���� ������ֱ�
	int new_p = make_leaf_page(tableid);
	pagenum_t n_p = b_M.frameArray[new_p].page_num;
//printf("insert into leaf after splitting 2 --- leaf pagenum = %ld\n",n_p);
	//tmpŰ�����Ϳ� �����͸� ����Ű�� �����͸� �����д� ? �� ---
	//�״ϱ� ����3���� ���� �ִµ� �ϳ��� �����°�� 4��¥���� �����Ҽ��ִ� temp�� �����
	//�װ� split�ؼ� �ΰ��� �ɰ��� �־��شٴ� ����

pthread_mutex_unlock(&b_M.frameArray[leaf].page_latch);
pthread_mutex_unlock(&b_M.frameArray[new_p].page_latch);

	//������ġ ã��
	insertion_index = 0;
	while (insertion_index < leaf_order && b_leaf.record[insertion_index].key < key)
		insertion_index++;

//printf("insert into leaf after splitting 3 -- insert index = %d\n",insertion_index);
	//������ġ�ΰ�� �ϳ������ leat�� ���� �����ؼ� �����ͼ� ���� 
	for (i = 0, j = 0; i < b_leaf.num_key; i++, j++) {
		if (j == insertion_index) j++;
		tmp_record[j].key = b_leaf.record[i].key;
		strcpy(tmp_record[j].val, b_leaf.record[i].val);
	}
	//�� ������ġ�� �־��ֱ�
	tmp_record[insertion_index].key = key;
	strcpy(tmp_record[insertion_index].val, val);


	//���� ������ ���� �ٽ� ä���ش�
	b_leaf. num_key= 0;

	//�������� ���ϱ�
	split = cut(leaf_order);
//printf("insert into leaf after splitting 4 -- split=%d\n",split);
	//���� ������ �ɰ��κ��� ������ �־��ֱ�
	for (i = 0; i < split; i++) {
		strcpy(b_leaf.record[i].val , tmp_record[i].val);
		b_leaf.record[i].key = tmp_record[i].key;
		b_leaf.num_key++;
	}
	//�� ������ �ɰ��κ��� ������ �κ��� �־��ֱ�
	for (i = split, j = 0; i <= leaf_order; i++, j++) {
		strcpy(b_M.frameArray[new_p].frame_p.record[j].val , tmp_record[i].val);
		b_new.record[j].key = tmp_record[i].key;
		b_new.num_key++;
	}

	//tmp���� free
	

	//�� ������ ���������� ��������ش�  ���� ������ ���������� ����Ű�� �ִ�
	//���� �κ��� ���θ���� ����ų�� �ֵ��� ������ ���� �ص־���
	b_new.right_left = b_leaf.right_left;
	b_leaf.right_left = n_p;

	//���� ������ �籸�������Ƿ� �������� ����ִ� �κе��� ������⶧����
	//�ű⿡ ���� ������ ���ش�
	for (i = b_leaf.num_key; i < leaf_order ; i++) {
		b_leaf.record[i].key = -1;
		
	}


	//���� ������ ���ʺκ� ����
	for (i = b_new.num_key; i < leaf_order ; i++) {
		b_new.record[i].key = -1;
		
	}

	//���� ������� ������ �θ�� ��������ְ� newŰ ���� 
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
	//n_p�� parent�� �޾ƿ�
	int page_parent = pageScan(tableid, n_p);
pthread_mutex_unlock(&b_M.frameArray[page_parent].page_latch);
	//left �ε������� ��ĭ�� ���������� �̷Ｍ �ڸ��� Ȯ���Ѵ�
	for (i = b_page_parent.num_key; i > left_index; i--) {
		b_page_parent.branch[i + 1].child = b_page_parent.branch[i].child;
		b_page_parent.branch[i + 1].key = b_page_parent.branch[i].key;
	}

	//Ű�� �����͸� ����
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
	//into parent�Լ����� old�� parent�� �Ѱ���
	int i, j, split;
	int64_t k_prime;
	
	//���� �ִ� ��� ������ֱ�
	int old_p = pageScan(tableid, old);

	//�ɰ��� �־��� �� ��� ������ֱ�
	int new_p = make_internal_page(tableid);
	pagenum_t n_p = b_M.frameArray[new_p].page_num;

pthread_mutex_unlock(&b_M.frameArray[old_p].page_latch);
pthread_mutex_unlock(&b_M.frameArray[new_p].page_latch);
 //printf("\ninsert into node after splitting 2 -- alloc complete : %d \n",new_p);
	/*
	 ���� �� Ű�� �����͸� ������ ��� ���� �ùٸ� ��ġ��
	 ������ �� �ִ� �ӽ� Ű�� �����͸� ����ʽÿ�.
	 �׷� ���� �� ��带 �����ϰ� Ű�� �������� ������ ���� ��忡,
	 ������ ������ �� ��忡 �����Ͻʽÿ�.
	 */
	 //�ӽð��� ����
	key_child * tmp_key_child = (key_child *)malloc(250 * sizeof(struct key_child));

	//������ ��ġ�� ����ش�
	for (i = 0, j = 0; i < b_old.num_key + 1; i++, j++) {
		if (j == left_index + 1) j++;
		tmp_key_child[j].child = b_old.branch[i].child;
	}
	for (i = 0, j = 0; i < b_old.num_key; i++, j++) {
		if (j == left_index+1) j++;
		tmp_key_child[j].key = b_old.branch[i].key;
	}

	//������ ��ġ�� ����
	tmp_key_child[left_index + 1].child = right;
	tmp_key_child[left_index+1].key = key;

	/* Create the new node and copy
	 * half the keys and pointers to the
	 * old and half to the new.
	 �� ��带 �����ϰ� Ű�� �������� ������ ���� ��忡, ������ ������ �� ��忡 �����Ͻʽÿ�.
	 */
	 //�ɰ��� ��ġ ã��
	split = cut(node_order);
	//���� ��� ����ֱ�
	b_old.num_key = 0;

	//�����ִ� ��� ä���
	for (i = 0; i < split-1; i++) {
		b_old.num_key++;
		b_old.branch[i].key = tmp_key_child[i].key;
		b_old.branch[i].child= tmp_key_child[i].child;
		
	}

	//������ ���� �ڽ��� �ڽ��� ���� ������ �׳� �ø��� �۾��� ������ ����x
	//�׷��� ���ͳ��� ��� �ڽ��� ���� �ö󰡰� �ǹǷ�
	//�ڽ��� �ڽ����� ������ �ʿ�


	k_prime = tmp_key_child[split-1].key;//�ö� ģ���� k_primed�� ����

	
	//���θ��� ���ä���
	b_new.num_key = 0;
	//split ���� ���� ����
	for (++i, j = 0; i <= node_order; i++, j++) {
		b_new.num_key++;
		b_new.branch[j].key = tmp_key_child[i].key;
		b_new.branch[j].child = tmp_key_child[i].child;
		
	}
	
	//�ø� Ű�� ���� ����Ű�� �ִ� �ڽ��� ���θ��� ����� right_left(rightmost)�� ����Ű�� ����
	b_new.right_left= tmp_key_child[split-1].child;

	//���θ��� ��� �θ������ֱ�
	b_new.parent = b_old.parent;

	//���θ��� ����� �ڽ��� �θ����� ������
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

	return insert_into_parent(tableid,old, k_prime, n_p);//�ö� ģ���� �����ؼ� �θ� ����
}


int insert_into_parent(int tableid,pagenum_t l, int64_t key, pagenum_t right) {
//printf("\ninsert into parent 1\n");
	int left_index;
	pagenum_t p_page;

	int left = pageScan(tableid, l);
	p_page = b_left.parent;
pthread_mutex_unlock(&b_M.frameArray[left].page_latch);

//printf("insert into parent 2 : left=%d,parent page=%ld\n",left,p_page);

	//����Ʈ�� �־���ϴ� ���(�� left�� ��Ʈ(internal)���� ���)
	if (p_page == 0) {
//printf("insert into new root call\n");
		clearPin(left);
		return insert_into_new_root(tableid,l, key, right);
	}

	//�ƴѰ�� �θ� �޾ƿ´�
	int parent = pageScan(tableid, p_page);
	pthread_mutex_unlock(&b_M.frameArray[parent].page_latch);
	//left�� �θ��ʿ��� ���° �ε����� �ִ��� Ȯ��
//printf("insert into parent : parent = %ld\n",p_page);
	left_index = get_left_index(tableid,p_page, l);

	//�θ��������� �÷��ش�
	if (b_parent.num_key < node_order) {
//printf("insert into node call\n");
		clearPin(left);
		clearPin(parent);

		return insert_into_node(tableid,p_page, left_index, key, right);
	}
	 //�θ�� �ϳ� �÷ȴµ� �ű⼭�� ������� �ٽ� �ɰ������
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
	//�� Ű�� ����������� �ִ� ������������ �����´�
	int find_p = find_page_2(tableid,key);

//printf("db_find --- find page success, find_p = %d\n",find_p);
	//��Ʈ���� ���
	if (find_p == FAIL) {
//printf("db_find- empty\n");
		//clearPin(find_p);
		return FAIL;
	}
//printf("?..\n");

	//Ű�� �߰��ϸ� �����
	for (i = 0; i < b_M.frameArray[find_p].frame_p.num_key; i++)
	{
//printf("[%d]check... ",i);
		if (b_M.frameArray[find_p].frame_p.record[i].key == key) break;
	}
//printf("now i = %d and num_key =%d\n",i,b_M.frameArray[find_p].frame_p.num_key);
	//�ε����� ������ �ƴ϶�� ã�����Ѱ� - break�� �������� �ƴ϶�� ��
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
	//���� �޾ƿ� �������� null�̶�� ��������
	//�ִٸ� �� ������ ������ key�� ��ġ�ϴ� ���� ã�´�
	//for ������ Ű�� �ε����� ã�´� i 5 
	//�� �ε����� numkey�� ���ٸ� ��ã���� --- -1����
	//�ƴ϶�� val�� �����ϰ�  0�̷�
}



int find_page_2(int tableid,int64_t key) {

//printf("\nfind page 1\n");
	int i = 0;
	//����� �޾ƿ´�
	int head = pageScan(tableid, 0);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
//printf("0");
	//��Ʈ�� �ϰ��
	if (b_head.root_page == 0) {
//printf("find page - empty\n");
		clearPin(head);


		return FAIL;
	}

	//��Ʈ��������
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
	//while �� �����ٴ°� ���� ���� ����
	//�̶� ��ȯ�Ǵ� ���� ���������� ���� Ű�� ������� ���� �ְ� ���� ���� �ִ�
	//���� ��� 9�� ã�� ���, 7�̻� 10�̸��� Ű���� ����־���ϴ� �������� ��ȯ�Ǿ
	//�ű⿡ 9�� �ִٴ� ������ x
pthread_mutex_unlock(&b_M.frameArray[trace].page_latch);
	int find = pageScan(tableid, tmp);
//printf("find page - 4\n");

	clearPin(trace);

	clearPin(find);
pthread_mutex_unlock(&b_M.frameArray[find].page_latch);
	return find;
}


