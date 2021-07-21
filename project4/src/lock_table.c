#include "lock_table.h"



typedef struct lock_t {
	
	lock_t *pre;
	lock_t *next;
	pthread_cond_t cond;
	HashTable * hash_ptr;

}lock_t;


typedef struct Node {
	int table_id;
	int record_id;//key

	lock_t * tail; //헤드 락
	lock_t * head; //테일 락

	Node * next;

}Node;

typedef struct HashNode {
	Node * nodeList; //Node 배열을 가리키는 포인터
	int key;
	int node_num;

}HashNode;


typedef struct HashTable {
	HashNode ** table;//HashNode 배열을 가리키는 포인터
	int size;
}HashTable;




int init_lock_table()
{
//printf("\ninit lock table start\n");
	//hash table create
	hash_t = createHashTable(30);
	if (!hash_t) {
		printf("init null error\n");
		return FAIL;
	}

	//mutex,cond init
	pthread_mutex_init(&lock_table_latch, NULL);
	
//hashPrint(hash_t);
//printf("init lock table complete\n");
	return SUCCESS;
}


lock_t* lock_acquire(int table_id, int64_t key)
{
//printf("\nlock acquire start - lock / table id = %d,key = %ld\n",table_id,key);
	//뮤텍스 락
	pthread_mutex_lock(&lock_table_latch);

	//새 락 오브젝트 생성 및 초기화
	lock_t * new_lock = (lock_t*)malloc(sizeof(struct lock_t));
	new_lock->pre = NULL;
	new_lock->next = NULL;
	
	new_lock->hash_ptr=hash_t;
	//new_lock->cond=PTHREAD_COND_INITIALIZER;
	pthread_cond_init(&new_lock->cond,NULL);
	
	//현재테이블에 해당레코드를 가진 노드가 존재하지 않는다면
	//해시테이블에 노드를 추가해준후 락 리스트에 달아준다
	Node* find = hashSearch(hash_t, table_id,key);
	if(!find) {
//printf("not find-hash insert\n");
		hashInsert(hash_t, table_id, key);
		find = hashSearch(hash_t, table_id, key);
	}

	//노드추가후 재시도 후 다시 실패시에는 fail
	if (!find) {
		pthread_mutex_unlock(&lock_table_latch);
		printf("lock acquire fail -1 \n");
		return NULL;
	}

	//존재한다면 해당노드뒤에 락 추가
	lock_t * tmp = find->head->next;
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

	//이미 해당 레코드가 잡혀있다면, 즉 자신이 유일한 lock이 아닌경우 sleep
	if (find->head->next != find->tail->pre) {
//printf("cond wait...please sleep\n");
	pthread_cond_wait(&new_lock->cond,&lock_table_latch);
	}
	
	//뮤텍스 언락
//printf("lock acquire : mutex unlock - finish\n");
	pthread_mutex_unlock(&lock_table_latch);
	return new_lock;
}

int lock_release(lock_t* lock_obj)
{
	if(lock_obj==NULL)return FAIL;
//printf("\nlock release start --- mutex lock\n");
	pthread_mutex_lock(&lock_table_latch);
	//락 리스트에서 제거해준다
	//링크드 리스트 수정
//printf("lock release : lock list delete\n");
	lock_obj->pre->next = lock_obj->next;
	lock_obj->next->pre = lock_obj->pre;

	//후임자가 있다면 깨워준다.
	if (lock_obj->next->next!=NULL) {
//printf("lock release : signal!!!!!\n");
		pthread_cond_signal(&lock_obj->next->cond);
	}

	free(lock_obj);
//hashPrint(hash_t);
//printf("lock release finish --- mutex unlock\n");
	pthread_mutex_unlock(&lock_table_latch);
	return SUCCESS;
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
printf("create error : memeory allocation fail -2\n");
		return NULL;
	}

	//각 테이블포인터에 hashnode공간 할당 및 값 초기화
	for (int i = 0; i < size; i++)
	{
		hash_t->table[i] = (HashNode *)malloc(sizeof(struct HashNode));
		if (!hash_t->table[i]) {
printf("create error : memeory allocation fail -3\n");
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

	//만약 빈 노드리스트라면
	if (!tmp) {
//printf("search null : empty node list\n");
		return NULL;
	}

	//탐색 시작
	while (tmp) {
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

	//리해시 필요시 함수 호출
	
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

			printf("node<%d> %d,%d\n", node_cnt,tmp->table_id,tmp->record_id);
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


