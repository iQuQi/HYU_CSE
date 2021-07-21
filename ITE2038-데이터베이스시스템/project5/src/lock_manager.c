#include "lock_manager.h"
#include "trx_manager.h"
#include "buf_manager.h"

#include "file.h"



//lock abort
int lock_abort(lock_t * lock)
{
//printf("\nlock abort start\n");
	 
	//���� S��� ���� ��� �׳� ����
	if (lock->lock_mode == 0) {
//printf("lock abort : S\n");
		lock_release(lock);
	}
	//���� E��� ���� ��� undo�� ���� , �� old������ val�� ���� �� ����
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


//��� ����
lock_t* lock_acquire(int table_id, int64_t key, int trx_id, int lock_mode)
{
//printf("\nlock acquire start - lock/ table id = %d,key = %ld, trx id=%d,mode=%d\n",table_id,key,trx_id,lock_mode);

	//���ؽ� ��
	pthread_mutex_lock(&lock_table_latch);
//printf("lock-table mutex lock suc/ table id = %d,key = %ld, trx id=%d,mode=%d\n",table_id,key,trx_id,lock_mode);
	int ch = 0,ch2=0;
	int t_c;
//printf("before init\n");
	if (!hash_t)init_lock_table();
	
	//�������̺� �ش緹�ڵ带 ���� ��尡 �������� �ʴ´ٸ�
	//�ؽ����̺� ��带 �߰������� �� ����Ʈ�� �޾��ش�
//printf("before node find\n");
	Node* find = hashSearch(hash_t, table_id,key);
	if(!find) {
//printf("not find-hash insert\n");
		hashInsert(hash_t, table_id, key);
		find = hashSearch(hash_t, table_id, key);
	}

	//����߰��� ��õ� �� �ٽ� ���нÿ��� fail
	

	//���� ���ڵ��� ���� �� trx�� ���� �ִ��� �˻� -> s1 - w1 �̷���� /��� s1->w2-w1�̰� ����� ��Ȳ��
	ch = trx_duplicate(find->head->next, trx_id);
	//ch2= trx_error(find->head->next);
	//printf("lock?...\n");
	lock_t * tmp = find->head->next;
	lock_t * tmp2;
	lock_t * dead_tmp;
	//���� �ְ� �̹��� ������ read�϶�
	
	if (ch == 1) {
		//printf("i have this record lock already\n");
		if (lock_mode ==0) {
			//printf("my mode is %d\n", lock_mode);
			//S�� �ʿ��ϸ� ���� ������ ����� �� lock�� �����ش�.
			while (tmp->next != NULL) {
				if (tmp->owner_txn_id == trx_id) {
					//printf("find my trx's another lock\n");
					//�� trx �Ҽ��� �ٸ� lock�� �߰��ߴµ� �� �ٷ� �������� ���� �޷��� �ϴ� �Ŷ��(�ǳ��̶��)
					if (tmp->next->next == NULL) {
						//printf("can exchange me right now\n");
						if(tmp==NULL)printf("exchange but null error\n");
						pthread_mutex_unlock(&lock_table_latch);
						//printf("lock acquire : mutex unlock - finish\n");
						return tmp;
					}

					//S�� �ڿ� �� �ְ� �׵ڿ� �̹� S�� �������� �Ű� �� �װ� �̹����� tail������ �� ����
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
			//�տ� �ִ� �ڽ��� trx�� ��
			while (tmp->next != NULL) {
				if (tmp->owner_txn_id == trx_id) break;
				tmp = tmp->next;
			}

			//W�� �ʿ��ѵ� �տ� W�� ���������� �װ� ������
			if (tmp->lock_mode == 1 && tmp->next->next == NULL) {
				//printf("pre lock is W, return this\n");
				pthread_mutex_unlock(&lock_table_latch);
				//printf("lock acquire : mutex unlock - finish\n");
				return tmp;
			}

			//�տ� s�ϳ��� ������ s�� ��带 w�� �����ϰ� ������ -> w2 s1 w1 �ΰ�� w2 w1���� ������
			if (tmp->lock_mode == 0&&tmp->next->next==NULL) {
				if(tmp->pre->pre==NULL){
				//printf("pre lock is only s and first lock, return this\n");
				tmp->lock_mode = 1;
				pthread_mutex_unlock(&lock_table_latch);
				//printf("lock acquire : mutex unlock - finish\n");
				return tmp;
				}
				
			}

			//s�� ������ ������  �����ִ� s�� �ڷ� �Ű��ְ� ���� w�� ����
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
			

					//����� ������ abort �Ǵ� fail��Ȳ �߻�
				
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

						//���ؽ� ���
						
						pthread_mutex_unlock(&lock_table_latch);
						//printf("lock acquire : mutex unlock - Execption Finish\n");
						return tmp;

					}
					tmp2 = tmp2->pre;
				}
			}
		}
	}


	//�� �� ������Ʈ ���� �� �ʱ�ȭ
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


	//�����Ѵٸ� �ش���ڿ� �� �߰�
	tmp = find->head->next;
	while (1) {
//printf("lock list check....\n");
		//null�̶��
		if(!tmp) {
			//���� tmp�� head��� head�� null�̶�� �� �� ���� �� ����Ʈ�� �����
			if (tmp == find->head->next) {
//printf("lock insert : first time\n");
				//����ũ�� ����Ʈ�� ����
				new_lock->pre = find->head;
				new_lock->next = find->tail;

				find->head->next = new_lock;
				find->tail->pre = new_lock;
			}
			//������Ʈ�� ������� ���� ���
			else {
//printf("lock insert : insert back\n");
				//���� ��ũ�� ����Ʈ�� ����
				new_lock->pre = find->tail->pre;
				new_lock->next = find->tail;

				new_lock->pre->next = new_lock;
				find->tail->pre = new_lock;
			}
			break;
		}

		//null �϶����� ����Ʈ ���� �̵�
		tmp = tmp->next;
	}
//hashPrint(hash_t);

//�ش� trx�� lock����Ʈ�� �Ŵ޾��ش�.
	
	

	//�̹� ���� �ش� ���ڵ忡 ���� lock�� ��� �ִµ� �׵ڷ� �ٷ� ������ ���� ���� deadlock x
	//�� �׳� ��������� �����θ� ��

	if (new_lock->lock_mode == 0) {//S���
		//�տ� �ƹ��� ������  ��� �ʿ� ����
//printf("my mode is S and my pre lock's lock mode is %d and trx_id=%d\n",new_lock->pre->sleep,new_lock->pre->owner_txn_id);
	
		//�տ� �����ִµ�
		 if (new_lock->pre->pre!=NULL&&(new_lock->pre->sleep||new_lock->pre->lock_mode ==1))//���� �ڰ��ְų� �����ִµ� E����϶��� ����ش� 
		{

			dead_tmp=new_lock->pre;
			new_lock->sleep = 1;
			ch=checkDead(dead_tmp, new_lock->owner_txn_id);
			

			//����� ������ abort �Ǵ� fail��Ȳ �߻�
			
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
	else if (new_lock->lock_mode == 1) {//E���
			//�տ� �ƹ��� ������  ��� �ʿ� ����
//printf("my mode is E\n");
		if (new_lock->pre->pre == NULL) {
			//printf("E mode : one lock - not sleep\n");
			t_c=trx_lock(trx_id, new_lock);
			
			
		}
		//�տ� ���� ������
		else {
			dead_tmp=new_lock->pre;

				new_lock->sleep = 1;
				ch=checkDead(dead_tmp, new_lock->owner_txn_id);
			
			//����� ������ abort �Ǵ� fail��Ȳ �߻�
				
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
	


	


	//���ؽ� ���

	pthread_mutex_unlock(&lock_table_latch);
//printf("lock acquire : mutex unlock - finish\n");
	return new_lock;
}


//�����ִ°͵� ���� -> �ѹ��� S��� �ٱ���°� �߰�
//�� S�� ��� S�ε��� ��� ���޾� ����� E�� ��� �ϳ��� ����� �ɵ�
int lock_release(lock_t* lock_obj)
{

	//�� ����Ʈ���� �������ش�
	//��ũ�� ����Ʈ ����
	if (!hash_t)init_lock_table();
	if(lock_obj->pre->pre==NULL&&lock_obj->next->next==NULL){
	lock_obj->pre->next = NULL;
	lock_obj->next->pre = NULL;
	}
	else{
	lock_obj->pre->next = lock_obj->next;
	lock_obj->next->pre = lock_obj->pre;
	}

	
	//�ڽ��� ������ S�ε� �ڿ� �����ڰ� �ְ� �װ� W�϶� W�ϳ��� �����
	if (lock_obj->lock_mode == 0&& lock_obj->pre->pre == NULL && lock_obj->next->next != NULL && lock_obj->next->lock_mode == 1) {
		//printf("only S mode : wake one E lock\nE-signal!!\n");
		pthread_cond_signal(&lock_obj->next->cond);
		lock_obj->next->sleep=0;
	}
	//�ڽ��� E���
	else if (lock_obj->lock_mode == 1) {
		//�����ڰ� �ִµ� �װ� S����϶��� S��� �ε��� �����
		if (lock_obj->next->next != NULL && lock_obj->next->lock_mode == 0) {
			//printf("E mode : wake all S locks\n");
			lock_t * tmp = lock_obj->next;

			//S��� �ε��� ��� �����(�������ΰ�� �� ������x)
			while (tmp->lock_mode == 0 && tmp->next != NULL) {
				//printf("S-signal!!-tmp's owner=%d ",tmp->owner_txn_id);
				pthread_cond_signal(&tmp->cond);
				tmp->sleep=0;
				tmp = tmp->next;
			}
			printf("\n");
		}

		//�����ڰ� �ִµ� E��� �ΰ�� �ϳ��� �����.
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
	//�ؽ����̺� ����ü ���� �Ҵ�
	HashTable* hash_t;
	hash_t=(HashTable*)malloc(sizeof(struct HashTable));

	if(!hash_t){
printf("create error : memory allocation fail - 1\n");
		return NULL;
	}

	//���̺� ����ü ���� ������������ ���� �Ҵ�
	hash_t->size = size;
	hash_t->table = (HashNode**)malloc(sizeof(struct HashNode*)*size);

	if (!hash_t->table) {
//printf("create error : memeory allocation fail -2\n");
		return NULL;
	}

	//�� ���̺������Ϳ� hashnode���� �Ҵ� �� �� �ʱ�ȭ
	for (int i = 0; i < size; i++)
	{
		hash_t->table[i] = (HashNode *)malloc(sizeof(struct HashNode));
		if (!hash_t->table[i]) {
//printf("create error : memeory allocation fail -3\n");
			return NULL;
		}

		//3�� �ʱ�ȭ clear
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
	//���� �� ��帮��Ʈ���
	if (!tmp) {
//printf("search null : empty node list\n");
		return NULL;
	}

	//Ž�� ����
	while (tmp) {
//printf("search3\n");
		if (tmp->record_id == r_id && tmp->table_id==table_id) {
//printf("search success - find\n");
			return tmp;
		}
		tmp = tmp->next;
	}

	//Ž�� ����
//printf("search fail - not found\n");
	return NULL;

}


int hashInsert(HashTable * hash_t, int table_id,int64_t r_id){
//printf("\ninsert data start\n");
	
	//��帮��Ʈ�� ����Ű�� ������ �����
	HashNode* first;
	first = hash_t->table[getKey(r_id, hash_t->size)];

	//�� �������� ��帮��Ʈ�� ������
	Node* tmp;
	tmp = first->nodeList;

	//������ ��� ����
	Node *node = (Node*)malloc(sizeof(struct Node));
	if (!node) {
printf("Hashinsert error : memory alloc fail -1\n");
		return FAIL;
	}

	//5���ʱ�ȭ clear
	node->table_id = table_id;
	node->record_id = r_id;
	node->next = NULL;//�������
	node->head = (lock_t*)malloc(sizeof(struct lock_t));//�� ����Ʈ ���
	node->tail = (lock_t*)malloc(sizeof(struct lock_t));//�� ����Ʈ ����
	
	node->head->next = NULL;
	node->head->pre = NULL;

	node->tail->next = NULL;
	node->tail->pre = NULL;


	//���� ��帮��Ʈ�� ����ٸ� �׳� node�ٷ� ����
	if (!tmp) first->nodeList = node;

	//��尡 �����ߴٸ�
	else {
		Node* tmp2;
		tmp2 = first->nodeList;//���� ��� �ӽ� ����
		first->nodeList = node;//�� �տ� ����
		node->next = tmp2;//�ش� ���ڿ� �����ִ� ��� ����
	}

	//��� ���� ����
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



