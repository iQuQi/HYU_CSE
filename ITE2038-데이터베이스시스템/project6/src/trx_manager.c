#include "file.h"
#include "trx_manager.h"
#include "lock_manager.h"
#include "buf_manager.h"
#include <stdlib.h>
#include <time.h>


int trx_init()
{
//printf("\ntrx init start\n");
	pthread_mutex_init(&trx_latch, NULL);
	//���� �Ŵ��� �ʱ�ȭ �ϸ鼭 ���ÿ� ����ߵɰ� ������ 
	T_M.trx_table_head = (Trx*)malloc(sizeof(struct Trx));
	T_M.trx_table_tail = (Trx*)malloc(sizeof(struct Trx));
	if (!T_M.trx_table_head)return FAIL;
	if (!T_M.trx_table_tail)return FAIL;

	//��� �ʱ�ȭ
	T_M.trx_table_head->trx_id = HEAD;
	T_M.trx_table_head->next = NULL;
	T_M.trx_table_head->pre = NULL;
	T_M.trx_table_head->trx_lock_head=NULL;

	//���� �ʱ�ȭ
	T_M.trx_table_tail->trx_id = TAIL;
	T_M.trx_table_tail->next = NULL;
	T_M.trx_table_tail->pre = NULL;
	T_M.trx_table_tail->trx_lock_head=NULL;
//printf("trx init end\n");
	return SUCCESS;
}


int trx_begin()
{
//printf("\ntrx begin start\n");
	pthread_mutex_lock(&trx_latch);
//printf("\ntrx begin mutex lock\n");

	//�� trx����
	Trx *new_trx=(Trx *)malloc(sizeof(struct Trx));
	if (!new_trx) {
		//printf("trx begin malloc error\n"); 
		pthread_mutex_unlock(&trx_latch);
		//printf("trx begin FAIL mutex unlock\n");
		return 0;//fail
	}

	//trx �Ŵ��� �ʱ�ȭ�� �ʿ��ϸ� �ϰ� �۷ι� trx id����
	//�̰� �ʱ�ȭ���� 0���� �Ǿ��ִ��� Ȯ��
	if (T_M.global_trx_id == 0) {
		//printf("T_M init\n");
		trx_init();
		
	}
	

	//�۷ι� ���̵� ���� ����
	T_M.global_trx_id++;

	//begin �α� ����
	log_BCR(T_M.global_trx_id,BEGIN);
	

	//�� trx �ʱ�ȭ
	new_trx->trx_id = T_M.global_trx_id;
	new_trx->state = 1;
	new_trx->lastLSN = -1;
	new_trx->trx_lock_head = NULL;
	
	if(T_M.trx_table_head->next==NULL){
	//printf("trx_begin init trx first\n");
		new_trx->next = T_M.trx_table_tail;
		new_trx->pre = T_M.trx_table_head;
		T_M.trx_table_head->next=new_trx;
		T_M.trx_table_tail->pre=new_trx;
		
	}
	else{
		//printf("trx_begin init trx not first\n");
		new_trx->next = T_M.trx_table_tail;
		new_trx->pre = T_M.trx_table_tail->pre;
		T_M.trx_table_tail->pre->next=new_trx;
		T_M.trx_table_tail->pre=new_trx;	
	}




//printf("trx begin end\n");
	pthread_mutex_unlock(&trx_latch);
//printf("trx begin mutex unlock\n");
	return new_trx->trx_id ;//success
}


int trx_commit(int trx_id)
{
//printf("\ntrx commit start\n");
	pthread_mutex_lock(&trx_latch);
//printf("trx commit mutex first lock\n");

	//�ش� trx�� ���̺��� ã�ƿ´�
	Trx* tmp = trx_find(trx_id);
	if (!tmp) {
		pthread_mutex_unlock(&trx_latch);
		//printf("trx commit FAIL mutex unlock\n");
		return 0;//fail
	}
//printf("commit 1\n");
	pthread_mutex_unlock(&trx_latch);
//printf("trx commit SUCCESS mutex first unlock\n");
	//trx�� �޷��ִ� lock���� ���ʷ� ���������ش�.
	lock_t * tmp_l;
	lock_t * lock_free = tmp->trx_lock_head;


	pthread_mutex_lock(&lock_table_latch);
//printf("trx commit lock table mutex lock\n");
	while (lock_free) {
		tmp_l = lock_free->trx_next_lock;
		lock_release(lock_free);
		lock_free = tmp_l;
	}

	pthread_mutex_unlock(&lock_table_latch);
//printf("trx commit lock table mutex unlock\n");

	
//printf("commit 2\n");
	pthread_mutex_lock(&trx_latch);
//printf("trx commit mutex second lock\n");

	tmp->next->pre = tmp->pre;
	tmp->pre->next = tmp->next;
	free(tmp);

//printf("trx commit SUCCESS end\n");
	pthread_mutex_unlock(&trx_latch);

	//Ŀ�� �α� ����
	log_BCR(trx_id, COMMIT);
	

	//���� ���� ��� �α׸� ���ۿ��� �α� ���Ϸ� ��� ������
	//�α� ���ۿ� �ִ� �α׷��ڵ带 �� ���Ϸ� �����ش�
	for (int i = log_M.head; i != -1; i = log_M.logRecords[i].next)
	{
		if (log_M.logRecords[i].type == COMMIT || log_M.logRecords[i].type == ROLLBACK || log_M.logRecords[i].type == BEGIN) {
			log_to_file_BCR(trx_id, log_fd, &log_M.logRecords[i].frame_BCR);
			log_M.logRecords[i].next = -1;
			log_M.logRecords[i].pre = -1;
		}
		else if (log_M.logRecords[i].type == UPDATE) {
			log_to_file_UP(trx_id, log_fd, &log_M.logRecords[i].frame_update);
			log_M.logRecords[i].next = -1;
			log_M.logRecords[i].pre = -1;
		}
		else {
			log_to_file_COM(trx_id, log_fd, &log_M.logRecords[i].frame_compensate);
			log_M.logRecords[i].next = -1;
			log_M.logRecords[i].pre = -1;
		}
	}
	//���нÿ� 0�� ��ȯ

//printf("trx commit SUCCESS mutex second unlock\n");
	return trx_id;
}


//�ڽ��� ��ٸ��� �ִ� lock�� ���� trx�� �ٸ� lock�� �ϳ��� �ڽ��� �Ǵٸ� lock�� ��ٸ��� �ִ� deadlock��Ȳ�� �����Ǹ� �ڽ��� abort�Ѵ�
//���ؽ� ��� ������ �� ����
int checkDead(lock_t* front,int back_id)
{
	
//printf("\ncheck dead start , front&back=%d,%d\n",front->owner_txn_id,back_id);
	int Dead = 0,ch,ch2,cnt=0,front_id,cycle=0;
	pthread_mutex_lock(&trx_latch);
//printf("check dead mutex lock\n");
	//�ش� trxã�ƿ���
	while(front->pre!=NULL){
	front_id=front->owner_txn_id;
	
//printf("\ncheck dead continue..... , front&back=%d,%d\n",front->owner_txn_id,back_id);
	Trx* find = trx_find(front_id);
	if (!find) {
		//printf("check Dead : cannot find trx in the trx table\n");
		
		continue;
	}

	//���� ��ٸ��� �ִ� trx�� id�� trx �Ŵ����� �ش�trx�� lock����Ʈ�� ��ȸ�Ѵ�.
	//�� lock�� wait�ε� �ڽ��� lock�� ��ٸ��� �ִ��� �ϳ��ϳ� ���󰡸鼭 �˻�
	lock_t * tmp = find->trx_lock_head;
	while (tmp) {
		cnt=0;
		cnt++;
//printf("check dead [%d]'s lock on front id trx\n",cnt);
		//��ٸ��� �ִ� ���ε� �װ� �ڽ��� ��ٸ��� ���
		ch = trx_dead_check(tmp, back_id);
		ch2=trx_cycle(tmp,back_id);


		if (ch ==-1) {
			//printf("check dead : FAIL-try_dead_check \n");
			continue;
		}

		else if (tmp->sleep&&ch>0) {
		//printf("dead check : same line!!!\n");
			Dead = 1;
			break;
		}
		

		if(ch2==ABORT) {
			Dead=1;
			

			//printf("not cycle but dead\n");

			break;
		}
		else if(ch2==CYCLE){		
			Dead=1;

			//printf("cycle\n");
			break;

		}
		tmp=tmp->trx_next_lock;
	}
		front=front->pre;
	}
	//����� ��Ȳ�� �����Ȱ�� abort ����
	if (Dead) {
		trx_abort(back_id);

		//printf("check dead : abort complete\n");
	}
	
	
	pthread_mutex_unlock(&trx_latch);
//printf("check dead mutext unlock-finish\n");

	if(Dead||cycle) return ABORT ;
	return SUCCESS;
}

//trx ���̺��� ����� �ش� trx�� lock����Ʈ�� ���󰡸鼭 lockAbort����
//��� ���ؽ� ����Ƶ� �� -> ����� �˻��Լ����� �̹� lock
int trx_abort(int trx_id)
{
//printf("\nabort start\n");
	//�ش�trx�� ���̺��� ã�ƿ´�
	Trx* tmp = trx_find(trx_id);
	if (!tmp) {
		//printf("abort : trx not find - FAIL\n");
		return FAIL;
	}

	//�ش� trx�� �� ����Ʈ�� ���󰡸鼭 lock_abort
	lock_t * tmp_l;
	lock_t * abort = tmp->trx_lock_head;
	while (abort) {
		tmp_l = abort->trx_next_lock;
		lock_release(abort);
		abort = tmp_l;

	}

	//�ش� trx�� ���̺��� �����
	tmp->next->pre = tmp->pre;
	tmp->pre->next = tmp->next;
	free(tmp);


	//compensate log�߱�
	int trace = tmp->lastLSN;
	while (1) {

		//�ش� ���ڵ��� preLSN�� ���󰡸鼭 compensate ����� - update log�� ��쿡��

		//-1�ΰ�� �ش� trx�� ���ڵ尡 ���̻� ���ٴ� ��
		//trace=
	}


	//������ �ٳ����� rollback �߱�
	log_BCR(trx_id,ROLLBACK);

	//�α� ���ۿ� �ִ� �α׷��ڵ带 �� ���Ϸ� �����ش�
	for (int i = log_M.head; i !=-1; i=log_M.logRecords[i].next)
	{
		if (log_M.logRecords[i].type == COMMIT || log_M.logRecords[i].type == ROLLBACK || log_M.logRecords[i].type == BEGIN) {
			log_to_file_BCR(trx_id, log_fd, &log_M.logRecords[i].frame_BCR);
			log_M.logRecords[i].next = -1;
			log_M.logRecords[i].pre = -1;
		}
		else if (log_M.logRecords[i].type == UPDATE) {
			log_to_file_UP(trx_id, log_fd, &log_M.logRecords[i].frame_update);
			log_M.logRecords[i].next = -1;
			log_M.logRecords[i].pre = -1;
		}
		else {
			log_to_file_COM(trx_id, log_fd, &log_M.logRecords[i].frame_compensate);
			log_M.logRecords[i].next = -1;
			log_M.logRecords[i].pre = -1;
		}
	}

//printf("abort end\n");
	return SUCCESS;
}

//�ش� trx�� �޾ƿ� ���� �޾��ش�.
//���ؽ��� �ϰ� ������ ����
int trx_lock(int trx_id, lock_t * lock)
{
//printf("\ntrx lock start\n");
	//�ش� trx�� ã�ƿ´�
	Trx* find = trx_find(trx_id);
	if(!find) return FAIL;
//printf("trx lock 1\n");
	//����ƮŽ��
	lock_t * find_l = find->trx_lock_head;

	if(find_l==NULL){
//printf("trx lock , find_l is NULL-> first this trx's lock\n");
		find->trx_lock_head=lock;
		lock->trx_next_lock=NULL;
	}
	else{
//printf("trx lock , not first this trx's lock\n");
		while ( find_l->trx_next_lock!= NULL) {
			find_l = find_l->trx_next_lock;
		}

	//while�� �������Դٴ°� �ڽ��� ������ lock�̶�� �� -> ���⿡ �� ���� �޾��ش�
		find_l->trx_next_lock = lock;
		lock->trx_next_lock = NULL;
	}

//printf("trx lock end\n");
	
	return SUCCESS;
}


//���ؽ� ���ʿ� - �긦 ȣ���ϴ� �Լ��ʿ��� �̹� �������
Trx * trx_find(int trx_id) {
//printf("\ntrx find start\n");
if(T_M.global_trx_id==0) {
	//printf("empty Trx table\n");
	return NULL;

}
	//�ش� trx�� ã�´�
	int ch = 0;
	Trx *tmp = T_M.trx_table_head->next;
	if(tmp==NULL) return NULL;
	while (tmp != T_M.trx_table_tail) {
		if (tmp->trx_id == trx_id) {
			ch = 1;
			break;
		}
		tmp = tmp->next;
	}
	//���ٸ� ����
	if (!ch) {
		//printf("cannot find this trx in the trx table\n");
		return NULL;
	}
//printf("trx find end -found!!\n");
	return tmp;
}
int trx_cycle(lock_t* obj,int back_id){
	//find acquireds
if(obj==NULL)return FAIL;
//sleep(1);
int id;
lock_t * first;
lock_t * tmp;
int wait=obj->owner_txn_id;

if(!obj->sleep)return SUCCESS;
	while(obj->pre!=NULL){
		if(!obj->sleep){
	
			id=obj->owner_txn_id;
			if(id==back_id){
					
			
					return ABORT;
			}

			first=trx_find(id)->trx_lock_head;
			while(first){

				if(first->sleep){

					tmp=first->pre;
					while(tmp->pre!=NULL){
					if(!tmp->sleep&&tmp->owner_txn_id==back_id) {

	
						return CYCLE;
					}
					tmp=tmp->pre;
					}
					
				}
				first=first->trx_next_lock;
			}
		}
		obj=obj->pre;
	}

}



//�ڽ��� ���� ��ٸ��� �� Ȯ��
int trx_dead_check(lock_t * obj,int back_id)
{
//printf("\ntrx dead check start\n");
	if(obj==NULL) {
//printf("trx dead check : obj == NULL\n");
		return -1;
	}
	if(!obj->sleep)return 0;
	Node * myNode = obj->node_ptr;
	if(myNode ==NULL){
//printf("trx dead check : myNode== NULL\n");
		return -1;
	}		
	lock_t * tmp = myNode->head->next;//�׷��ڵ��� �� ����Ʈ�� ù lock ������Ʈ
	int cnt = 0;
//printf("trx dead check 1\n");
	if (!tmp) {
//printf("trx dead check FAIL end: empty node\n");
		return 0;
	}


	while (tmp->next != NULL&&tmp!=obj) {
		if (tmp->owner_txn_id == back_id) {
			cnt++;
		}
		tmp = tmp->next;
	}

	if (cnt != 0) {
		//printf("trx dead check ABORT end\n");
		return cnt;
	}
	
//printf("trx dead chech SUCCESS end : cnt=%d\n",cnt);
	return cnt;
}

int trx_duplicate(lock_t * tmp,int back_id)
{
//printf("\ntrx dup check start\n");
	if(tmp==NULL) {
//printf("trx dup check : obj == NULL\n");
		return -1;
	}	
	
	int cnt = 0;
//printf("trx dup check 1\n");



	while (tmp->next != NULL) {
		if (tmp->owner_txn_id == back_id) {
			cnt++;
		}
		tmp = tmp->next;
	}

	if (cnt > 1) {
		//printf("trx dup DETECT cnt=%d \n",cnt);
		return cnt;
	}
	
//printf("trx dup chech SUCCESS end : cnt=%d\n",cnt);
	return cnt;
}

int trx_error(lock_t* obj){
//printf("sleep error check\n");
	if(obj==NULL) return 1;
//printf("why?..\n");
     	while(obj->next->next!=NULL){
//printf("error check...\n");
		if(obj->sleep&&!obj->next->sleep){
//printf("sleep error DETECT\n");
			return -1;
		}	

		obj=obj->next;
	}
//printf("no sleep ERROR\n");
	return 1;

}

void printTrx()
{
printf("\nprint Trx begin\n");
if(T_M.global_trx_id==0) return;
	Trx * start = T_M.trx_table_head->next;
	while (start->next) {
		if(start->trx_id!=TAIL) {printf("Trx id=%d \n", start->trx_id);
		printTrx_lock(start->trx_lock_head);
		}
		start = start->next;
		printf("\n");
	
	}

}

void printTrx_lock(lock_t* head)
{
	printf("trx lock list: ");
	while (head) {
		printf("lockmode=%d isSleep%d", head->lock_mode, head->sleep);
		printf(" record id=%ld table id%d\n", head->node_ptr->record_id, head->node_ptr->table_id);
		printf("\n");
		head=head->trx_next_lock;
	}
}






