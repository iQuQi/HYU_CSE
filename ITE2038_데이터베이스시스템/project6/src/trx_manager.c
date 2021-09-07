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
	//버퍼 매니저 초기화 하면서 동시에 해줘야될거 같은데 
	T_M.trx_table_head = (Trx*)malloc(sizeof(struct Trx));
	T_M.trx_table_tail = (Trx*)malloc(sizeof(struct Trx));
	if (!T_M.trx_table_head)return FAIL;
	if (!T_M.trx_table_tail)return FAIL;

	//헤드 초기화
	T_M.trx_table_head->trx_id = HEAD;
	T_M.trx_table_head->next = NULL;
	T_M.trx_table_head->pre = NULL;
	T_M.trx_table_head->trx_lock_head=NULL;

	//테일 초기화
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

	//새 trx생성
	Trx *new_trx=(Trx *)malloc(sizeof(struct Trx));
	if (!new_trx) {
		//printf("trx begin malloc error\n"); 
		pthread_mutex_unlock(&trx_latch);
		//printf("trx begin FAIL mutex unlock\n");
		return 0;//fail
	}

	//trx 매니저 초기화가 필요하면 하고 글로벌 trx id증가
	//이거 초기화전에 0으로 되어있는지 확인
	if (T_M.global_trx_id == 0) {
		//printf("T_M init\n");
		trx_init();
		
	}
	

	//글로벌 아이디 변수 증가
	T_M.global_trx_id++;

	//begin 로그 생성
	log_BCR(T_M.global_trx_id,BEGIN);
	

	//새 trx 초기화
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

	//해당 trx을 테이블에서 찾아온다
	Trx* tmp = trx_find(trx_id);
	if (!tmp) {
		pthread_mutex_unlock(&trx_latch);
		//printf("trx commit FAIL mutex unlock\n");
		return 0;//fail
	}
//printf("commit 1\n");
	pthread_mutex_unlock(&trx_latch);
//printf("trx commit SUCCESS mutex first unlock\n");
	//trx에 달려있는 lock들을 차례로 해제시켜준다.
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

	//커밋 로그 생성
	log_BCR(trx_id, COMMIT);
	

	//리턴 전에 모든 로그를 버퍼에서 로그 파일로 모두 내린다
	//로그 버퍼에 있는 로그레코드를 다 파일로 내려준다
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
	//실패시에 0을 반환

//printf("trx commit SUCCESS mutex second unlock\n");
	return trx_id;
}


//자신이 기다리고 있는 lock의 주인 trx의 다른 lock중 하나가 자신의 또다른 lock을 기다리고 있는 deadlock상황이 감지되면 자신을 abort한다
//뮤텍스 잡고 끝나기 전 해제
int checkDead(lock_t* front,int back_id)
{
	
//printf("\ncheck dead start , front&back=%d,%d\n",front->owner_txn_id,back_id);
	int Dead = 0,ch,ch2,cnt=0,front_id,cycle=0;
	pthread_mutex_lock(&trx_latch);
//printf("check dead mutex lock\n");
	//해당 trx찾아오기
	while(front->pre!=NULL){
	front_id=front->owner_txn_id;
	
//printf("\ncheck dead continue..... , front&back=%d,%d\n",front->owner_txn_id,back_id);
	Trx* find = trx_find(front_id);
	if (!find) {
		//printf("check Dead : cannot find trx in the trx table\n");
		
		continue;
	}

	//내가 기다리고 있는 trx의 id로 trx 매니저가 해당trx의 lock리스트를 순회한다.
	//그 lock가 wait인데 자신의 lock을 기다리고 있는지 하나하나 따라가면서 검사
	lock_t * tmp = find->trx_lock_head;
	while (tmp) {
		cnt=0;
		cnt++;
//printf("check dead [%d]'s lock on front id trx\n",cnt);
		//기다리고 있는 중인데 그게 자신을 기다리는 경우
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
	//데드락 상황이 감지된경우 abort 실행
	if (Dead) {
		trx_abort(back_id);

		//printf("check dead : abort complete\n");
	}
	
	
	pthread_mutex_unlock(&trx_latch);
//printf("check dead mutext unlock-finish\n");

	if(Dead||cycle) return ABORT ;
	return SUCCESS;
}

//trx 테이블에서 지우고 해당 trx의 lock리스트를 따라가면서 lockAbort실행
//얘는 뮤텍스 안잡아도 됨 -> 데드락 검사함수에서 이미 lock
int trx_abort(int trx_id)
{
//printf("\nabort start\n");
	//해당trx을 테이블에서 찾아온다
	Trx* tmp = trx_find(trx_id);
	if (!tmp) {
		//printf("abort : trx not find - FAIL\n");
		return FAIL;
	}

	//해당 trx의 락 리스트를 따라가면서 lock_abort
	lock_t * tmp_l;
	lock_t * abort = tmp->trx_lock_head;
	while (abort) {
		tmp_l = abort->trx_next_lock;
		lock_release(abort);
		abort = tmp_l;

	}

	//해당 trx를 테이블에서 지운다
	tmp->next->pre = tmp->pre;
	tmp->pre->next = tmp->next;
	free(tmp);


	//compensate log발급
	int trace = tmp->lastLSN;
	while (1) {

		//해당 레코드의 preLSN을 따라가면서 compensate 만들기 - update log인 경우에만

		//-1인경우 해당 trx의 레코드가 더이상 없다는 뜻
		//trace=
	}


	//무사히 다끝나면 rollback 발급
	log_BCR(trx_id,ROLLBACK);

	//로그 버퍼에 있는 로그레코드를 다 파일로 내려준다
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

//해당 trx에 받아온 락을 달아준다.
//뮤텍스락 하고 끝날때 해제
int trx_lock(int trx_id, lock_t * lock)
{
//printf("\ntrx lock start\n");
	//해당 trx를 찾아온다
	Trx* find = trx_find(trx_id);
	if(!find) return FAIL;
//printf("trx lock 1\n");
	//리스트탐색
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

	//while을 빠져나왔다는건 자신이 마지막 lock이라는 뜻 -> 여기에 새 락을 달아준다
		find_l->trx_next_lock = lock;
		lock->trx_next_lock = NULL;
	}

//printf("trx lock end\n");
	
	return SUCCESS;
}


//뮤텍스 불필요 - 얘를 호출하는 함수쪽에서 이미 잡고있음
Trx * trx_find(int trx_id) {
//printf("\ntrx find start\n");
if(T_M.global_trx_id==0) {
	//printf("empty Trx table\n");
	return NULL;

}
	//해당 trx를 찾는다
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
	//없다면 실패
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



//자신의 것을 기다리는 지 확인
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
	lock_t * tmp = myNode->head->next;//그레코드의 락 리스트의 첫 lock 오브젝트
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






