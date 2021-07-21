#include "lock_manager.h"
#include "trx_manager.h"
#include "buf_manager.h"

#include "file.h"



//lock abort
int lock_abort(lock_t * lock)
{
//printf("\nlock abort start\n");
	 
	//만약 S모드 였을 경우 그냥 삭제
	if (lock->lock_mode == 0) {
//printf("lock abort : S\n");
		lock_release(lock);
	}
	//만약 E모드 였을 경우 undo후 삭제 , 즉 old값으로 val값 변경 후 삭제
	else if (lock->lock_mode == 1) {
//printf("lock abort : E\n");
		if(lock->change){
		strcpy(b_M.frameArray[lock->recovery_p].frame_p.record[lock->recovery_r].val,lock->old_val);
		}
//printf("lock abort : undo success!!!!!--------------------------------------\n");
		lock_release(lock);
	}

	else {
//printf("lock abort FAIL end\n");
		return FAIL;
	}
//printf("lock abort SUCCESS end\n");
	return SUCCESS;
}



int init_lock_table()
{
//printf("\ninit lock table start\n");
	//hash table create
	hash_t = createHashTable(20);
	if (!hash_t) {
		//printf("init null error\n");
		return FAIL;
	}

	//mutex,cond init
	pthread_mutex_init(&lock_table_latch, NULL);
	
//hashPrint(hash_t);
//printf("init lock table complete\n");
	return SUCCESS;
}


//모드 설정
lock_t* lock_acquire(int table_id, int64_t key, int trx_id, int lock_mode)
{
//printf("\nlock acquire start - lock/ table id = %d,key = %ld, trx id=%d,mode=%d\n",table_id,key,trx_id,lock_mode);

	//뮤텍스 락
	pthread_mutex_lock(&lock_table_latch);
//printf("lock-table mutex lock suc/ table id = %d,key = %ld, trx id=%d,mode=%d\n",table_id,key,trx_id,lock_mode);
	int ch = 0,ch2=0;
	int t_c;
//printf("before init\n");
	if (!hash_t)init_lock_table();
	
	//현재테이블에 해당레코드를 가진 노드가 존재하지 않는다면
	//해시테이블에 노드를 추가해준후 락 리스트에 달아준다
//printf("before node find\n");
	Node* find = hashSearch(hash_t, table_id,key);
	if(!find) {
//printf("not find-hash insert\n");
		hashInsert(hash_t, table_id, key);
		find = hashSearch(hash_t, table_id, key);
	}

	//노드추가후 재시도 후 다시 실패시에는 fail
	

	//현재 레코드의 락을 이 trx이 갖고 있는지 검사 -> s1 - w1 이런경우 /대신 s1->w2-w1이건 데드락 상황임
	ch = trx_duplicate(find->head->next, trx_id);
	//ch2= trx_error(find->head->next);
	//printf("lock?...\n");
	lock_t * tmp = find->head->next;
	lock_t * tmp2;
	lock_t * dead_tmp;
	//갖고 있고 이번에 연산이 read일때
	
	if (ch == 1) {
		//printf("i have this record lock already\n");
		if (lock_mode ==0) {
			//printf("my mode is %d\n", lock_mode);
			//S가 필요하면 원래 잡힌걸 쓰면됨 그 lock을 돌려준다.
			while (tmp->next != NULL) {
				if (tmp->owner_txn_id == trx_id) {
					//printf("find my trx's another lock\n");
					//이 trx 소속의 다른 lock을 발견했는데 그 바로 다음것이 지금 달려고 하는 거라면(맨끝이라면)
					if (tmp->next->next == NULL) {
						//printf("can exchange me right now\n");
						if(tmp==NULL)printf("exchange but null error\n");
						pthread_mutex_unlock(&lock_table_latch);
						//printf("lock acquire : mutex unlock - finish\n");
						return tmp;
					}

					//S만 뒤에 쭉 있고 그뒤에 이번 S를 넣으려는 거고 즉 그게 이번꺼가 tail앞으로 들어갈 예정
					tmp2 = tmp;
					while (tmp2->lock_mode==0) {
						//printf("can exchange, lot of S\n");
						if (tmp2->next->next == NULL) {
							pthread_mutex_unlock(&lock_table_latch);
							//printf("lock acquire : mutex unlock - finish\n");
							return tmp;
						}
						tmp2 = tmp2->next;
					}
					break;
				}
				tmp = tmp->next;
			}
		}
		else {
			//printf("my mode is %d\n", lock_mode);
			//앞에 있는 자신의 trx의 락
			while (tmp->next != NULL) {
				if (tmp->owner_txn_id == trx_id) break;
				tmp = tmp->next;
			}

			//W가 필요한데 앞에 W가 잡혀있으면 그거 돌려줌
			if (tmp->lock_mode == 1 && tmp->next->next == NULL) {
				//printf("pre lock is W, return this\n");
				pthread_mutex_unlock(&lock_table_latch);
				//printf("lock acquire : mutex unlock - finish\n");
				return tmp;
			}

			//앞에 s하나만 있으면 s의 모드를 w로 변경하고 돌려줌 -> w2 s1 w1 인경우 w2 w1으로 돌려줌
			if (tmp->lock_mode == 0&&tmp->next->next==NULL) {
				if(tmp->pre->pre==NULL){
				//printf("pre lock is only s and first lock, return this\n");
				tmp->lock_mode = 1;
				pthread_mutex_unlock(&lock_table_latch);
				//printf("lock acquire : mutex unlock - finish\n");
				return tmp;
				}
				
			}

			//s가 여러개 있으면  원래있던 s를 뒤로 옮겨주고 재우고 w로 변경
			if (tmp->lock_mode == 0) {
			
				tmp2 = find->tail->pre;
				while (tmp2->lock_mode==0) {
					if (tmp2 == tmp) {
					//printf("there are many S in front of me!! move this to tail and return s->w and sleep\n");
						tmp->pre->next = tmp->next;
						tmp->next->pre = tmp->pre;

						tmp->pre = find->tail->pre;
						tmp->next = find->tail;
						find->tail->pre->next=tmp;
						find->tail->pre=tmp;
						
						
						tmp->sleep = 1;
					
						
						
				dead_tmp=tmp->pre;
				
				ch=checkDead(dead_tmp, tmp->owner_txn_id);
			

					//데드락 감지시 abort 또는 fail상황 발생
				
				if (ch == ABORT) {
					pthread_mutex_unlock(&lock_table_latch);
					//printf("lock acquire : mutex unlock - ABORT finish\n");
					return NULL;
				}
				

						tmp->lock_mode = 1;
				

				

						
						//printf("cond wait...please sleep\n");
						//printTrx();

	//for(int i=0;i<15;i++){
	//show_lock_list(i,1);
	//}
						
						while(tmp->sleep){
						ch=pthread_cond_wait(&tmp->cond, &lock_table_latch);
						}
						
						//printf("cond wake!... continue the function,ch=%d\n",ch);

						//뮤텍스 언락
						
						pthread_mutex_unlock(&lock_table_latch);
						//printf("lock acquire : mutex unlock - Execption Finish\n");
						return tmp;

					}
					tmp2 = tmp2->pre;
				}
			}
		}
	}


	//새 락 오브젝트 생성 및 초기화
	lock_t * new_lock = (lock_t*)malloc(sizeof(struct lock_t));
		
	new_lock->pre = NULL;
	new_lock->next = NULL;
	new_lock->lock_mode = lock_mode;
	new_lock->owner_txn_id = trx_id;
	new_lock->sleep = 0;
	new_lock->change=0;
	new_lock->recovery_r=-2;
	new_lock->trx_next_lock = NULL;
	new_lock->node_ptr = find;
	pthread_cond_init(&new_lock->cond, NULL);


	//존재한다면 해당노드뒤에 락 추가
	tmp = find->head->next;
	while (1) {
//printf("lock list check....\n");
		//null이라면
		if(!tmp) {
			//만약 tmp가 head라면 head가 null이라는 뜻 즉 아직 락 리스트가 비었음
			if (tmp == find->head->next) {
//printf("lock insert : first time\n");
				//더블링크드 리스트에 연결
				new_lock->pre = find->head;
				new_lock->next = find->tail;

				find->head->next = new_lock;
				find->tail->pre = new_lock;
			}
			//락리스트가 비어있지 않은 경우
			else {
//printf("lock insert : insert back\n");
				//더블 링크드 리스트에 연결
				new_lock->pre = find->tail->pre;
				new_lock->next = find->tail;

				new_lock->pre->next = new_lock;
				find->tail->pre = new_lock;
			}
			break;
		}

		//null 일때까지 리스트 따라서 이동
		tmp = tmp->next;
	}
//hashPrint(hash_t);

//해당 trx의 lock리스트에 매달아준다.
	
	

	//이미 내가 해당 레코드에 대한 lock을 잡고 있는데 그뒤로 바로 내가또 잡은 경우는 deadlock x
	//즉 그냥 재우지말고 깨워두면 됨

	if (new_lock->lock_mode == 0) {//S모드
		//앞에 아무도 없으면  재울 필요 없음
//printf("my mode is S and my pre lock's lock mode is %d and trx_id=%d\n",new_lock->pre->sleep,new_lock->pre->owner_txn_id);
	
		//앞에 누가있는데
		 if (new_lock->pre->pre!=NULL&&(new_lock->pre->sleep||new_lock->pre->lock_mode ==1))//앞이 자고있거나 깨어있는데 E모드일때만 재워준다 
		{

			dead_tmp=new_lock->pre;
			new_lock->sleep = 1;
			ch=checkDead(dead_tmp, new_lock->owner_txn_id);
			

			//데드락 감지시 abort 또는 fail상황 발생
			
			if (ch == ABORT) {
				lock_release(new_lock);
				pthread_mutex_unlock(&lock_table_latch);
				//printf("lock acquire : mutex unlock - ABORT finish\n");
				return NULL;
			}


			t_c=trx_lock(trx_id, new_lock);
			
			
			//printf("cond wait...please sleep\n");
			

			//printTrx();
	//for(int i=0;i<15;i++){
	//show_lock_list(i,1);
	//}
			//ch=pthread_cond_wait(&new_lock->cond, &lock_table_latch);
			
		
			//printf("cond wake!... continue the function\n");
			
		}
		else{
			//printf("S mode : only S lock - not sleep\n");
			t_c=trx_lock(trx_id, new_lock);
			
			
		}
		  
	}
	else if (new_lock->lock_mode == 1) {//E모드
			//앞에 아무도 없으면  재울 필요 없음
//printf("my mode is E\n");
		if (new_lock->pre->pre == NULL) {
			//printf("E mode : one lock - not sleep\n");
			t_c=trx_lock(trx_id, new_lock);
			
			
		}
		//앞에 누가 있으면
		else {
			dead_tmp=new_lock->pre;

				new_lock->sleep = 1;
				ch=checkDead(dead_tmp, new_lock->owner_txn_id);
			
			//데드락 감지시 abort 또는 fail상황 발생
				
				if (ch == ABORT) {
					lock_release(new_lock);
					pthread_mutex_unlock(&lock_table_latch);
					//printf("lock acquire : mutex unlock -ABORT finish\n");
					return NULL;
				}

				t_c=trx_lock(trx_id, new_lock);
				
				
				
			//printf("cond wait...please sleep\n");

			//printTrx();
	//for(int i=0;i<15;i++){
	//show_lock_list(i,1);
	//}
			
			while(new_lock->sleep){
				ch=pthread_cond_wait(&new_lock->cond, &lock_table_latch);
			}
			//printf("E:cond wait result , ch=%d\n",ch);
			//printf("cond wake!... continue the function\n");
		}
	}
	


	


	//뮤텍스 언락

	pthread_mutex_unlock(&lock_table_latch);
//printf("lock acquire : mutex unlock - finish\n");
	return new_lock;
}


//깨워주는것도 수정 -> 한번에 S모드 다깨우는거 추가
//즉 S인 경우 S인동안 계속 연달아 깨우고 E인 경우 하나만 깨우면 될듯
int lock_release(lock_t* lock_obj)
{

	//락 리스트에서 제거해준다
	//링크드 리스트 수정
	if (!hash_t)init_lock_table();
	if(lock_obj->pre->pre==NULL&&lock_obj->next->next==NULL){
	lock_obj->pre->next = NULL;
	lock_obj->next->pre = NULL;
	}
	else{
	lock_obj->pre->next = lock_obj->next;
	lock_obj->next->pre = lock_obj->pre;
	}

	
	//자신이 유일한 S인데 뒤에 후임자가 있고 그게 W일때 W하나만 깨운다
	if (lock_obj->lock_mode == 0&& lock_obj->pre->pre == NULL && lock_obj->next->next != NULL && lock_obj->next->lock_mode == 1) {
		//printf("only S mode : wake one E lock\nE-signal!!\n");
		pthread_cond_signal(&lock_obj->next->cond);
		lock_obj->next->sleep=0;
	}
	//자신이 E모드
	else if (lock_obj->lock_mode == 1) {
		//후임자가 있는데 그게 S모드일때는 S모드 인동안 깨운다
		if (lock_obj->next->next != NULL && lock_obj->next->lock_mode == 0) {
			//printf("E mode : wake all S locks\n");
			lock_t * tmp = lock_obj->next;

			//S모드 인동안 계속 깨우기(마지막인경우 더 깨우지x)
			while (tmp->lock_mode == 0 && tmp->next != NULL) {
				//printf("S-signal!!-tmp's owner=%d ",tmp->owner_txn_id);
				pthread_cond_signal(&tmp->cond);
				tmp->sleep=0;
				tmp = tmp->next;
			}
			printf("\n");
		}

		//후임자가 있는데 E모드 인경우 하나만 깨운다.
		else if(lock_obj->next->next != NULL && lock_obj->next->lock_mode == 1) {
			//printf("E mode : wake one W lock, W-signal!!\n");
			pthread_cond_signal(&lock_obj->next->cond);
			lock_obj->next->sleep=0;
		}
	}
	else if(lock_obj->lock_mode!=0&&lock_obj->lock_mode!=1){
	
		return FAIL;
	}


	free(lock_obj);
//hashPrint(hash_t);

	return SUCCESS;
}




void show_lock_list(int64_t key,int table_id){
//printf("show lock list start\n");
	Node* node = hashSearch(hash_t, table_id,key);
//printf("1");
	if(node==NULL){return; printf("node null\n");}

	if(node->head->next==NULL){ printf("node null\n");return;}


	printf("\nkey <%ld>'s lock list\n",key);
	lock_t * tmp=node->head->next;
	int cnt=0;
	while(tmp->next!=NULL){
		cnt++;
		if(tmp->next==NULL) break;
		printf("[%d] tmp->sleep=%d, tmp->lockmode=%d, tmp->owner_trx_id=%d\n",cnt,tmp->sleep,tmp->lock_mode,tmp->owner_txn_id);
		if(!trx_find(tmp->owner_txn_id)){
printf("cant find\n");
			
			return;
		}
		tmp=tmp->next;
	}

}

//////////////
//HASH FUNCT//
//////////////



HashTable * createHashTable(int size){
//printf("\ncreate table start\n");
	//해쉬테이블 구조체 공간 할당
	HashTable* hash_t;
	hash_t=(HashTable*)malloc(sizeof(struct HashTable));

	if(!hash_t){
printf("create error : memory allocation fail - 1\n");
		return NULL;
	}

	//테이블 구조체 안의 이차원포인터 공간 할당
	hash_t->size = size;
	hash_t->table = (HashNode**)malloc(sizeof(struct HashNode*)*size);

	if (!hash_t->table) {
//printf("create error : memeory allocation fail -2\n");
		return NULL;
	}

	//각 테이블포인터에 hashnode공간 할당 및 값 초기화
	for (int i = 0; i < size; i++)
	{
		hash_t->table[i] = (HashNode *)malloc(sizeof(struct HashNode));
		if (!hash_t->table[i]) {
//printf("create error : memeory allocation fail -3\n");
			return NULL;
		}

		//3개 초기화 clear
		hash_t->table[i]->key = i;
		hash_t->table[i]->node_num = 0;
		hash_t->table[i]->nodeList = NULL;

	}

//printf("hash table create complete\n");
	return hash_t;
}

Node* hashSearch(HashTable * hash_t,int table_id,int64_t r_id){
//printf("\nsearch start\n");
	Node * tmp = NULL;
	tmp = hash_t->table[getKey(r_id, hash_t->size)]->nodeList;
//printf("search2\n");
	//만약 빈 노드리스트라면
	if (!tmp) {
//printf("search null : empty node list\n");
		return NULL;
	}

	//탐색 시작
	while (tmp) {
//printf("search3\n");
		if (tmp->record_id == r_id && tmp->table_id==table_id) {
//printf("search success - find\n");
			return tmp;
		}
		tmp = tmp->next;
	}

	//탐색 실패
//printf("search fail - not found\n");
	return NULL;

}


int hashInsert(HashTable * hash_t, int table_id,int64_t r_id){
//printf("\ninsert data start\n");
	
	//노드리스트를 가리키는 포인터 갖고옴
	HashNode* first;
	first = hash_t->table[getKey(r_id, hash_t->size)];

	//그 포인터의 노드리스트를 가져옴
	Node* tmp;
	tmp = first->nodeList;

	//삽입할 노드 생성
	Node *node = (Node*)malloc(sizeof(struct Node));
	if (!node) {
printf("Hashinsert error : memory alloc fail -1\n");
		return FAIL;
	}

	//5개초기화 clear
	node->table_id = table_id;
	node->record_id = r_id;
	node->next = NULL;//다음노드
	node->head = (lock_t*)malloc(sizeof(struct lock_t));//락 리스트 헤드
	node->tail = (lock_t*)malloc(sizeof(struct lock_t));//락 리스트 테일
	
	node->head->next = NULL;
	node->head->pre = NULL;

	node->tail->next = NULL;
	node->tail->pre = NULL;


	//만약 노드리스트가 비었다면 그냥 node바로 삽입
	if (!tmp) first->nodeList = node;

	//노드가 존재했다면
	else {
		Node* tmp2;
		tmp2 = first->nodeList;//원래 노드 임시 저장
		first->nodeList = node;//맨 앞에 삽입
		node->next = tmp2;//해당 노드뒤에 원래있던 노드 연결
	}

	//노드 개수 증가
	first->node_num++;

//printf("insert data complete\n");
	return SUCCESS;
	
}




int hashPrint(HashTable * hash_t){
	int lock_cnt=0;
	int node_cnt=0;
printf("\nprint hash table data\n");
	for (int i = 0; i < hash_t->size; i++)
	{
		HashNode * first = hash_t->table[i];
		Node * tmp = first->nodeList;

		printf("Key %d : \n",i);
		while (tmp) {
			node_cnt++;

			printf("node<%d> %d,%ld\n", node_cnt,tmp->table_id,tmp->record_id);
			printf("<lock list> ");
			lock_t * tmp_l = tmp->head->next;
			while(tmp_l->next!=NULL){
				lock_cnt++;
				printf("%d ",lock_cnt);
				tmp_l=tmp_l->next;
			}
			printf("\n");
				
			tmp = tmp->next;
		}
		printf("\n");
	}
//printf("print hash table complete\n");
	return SUCCESS;
}

int getKey(int64_t r_id, int table_size) {

	return r_id % table_size;
}



