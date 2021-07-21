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

	lock_t * tail; //��� ��
	lock_t * head; //���� ��

	Node * next;

}Node;

typedef struct HashNode {
	Node * nodeList; //Node �迭�� ����Ű�� ������
	int key;
	int node_num;

}HashNode;


typedef struct HashTable {
	HashNode ** table;//HashNode �迭�� ����Ű�� ������
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
	//���ؽ� ��
	pthread_mutex_lock(&lock_table_latch);

	//�� �� ������Ʈ ���� �� �ʱ�ȭ
	lock_t * new_lock = (lock_t*)malloc(sizeof(struct lock_t));
	new_lock->pre = NULL;
	new_lock->next = NULL;
	
	new_lock->hash_ptr=hash_t;
	//new_lock->cond=PTHREAD_COND_INITIALIZER;
	pthread_cond_init(&new_lock->cond,NULL);
	
	//�������̺� �ش緹�ڵ带 ���� ��尡 �������� �ʴ´ٸ�
	//�ؽ����̺� ��带 �߰������� �� ����Ʈ�� �޾��ش�
	Node* find = hashSearch(hash_t, table_id,key);
	if(!find) {
//printf("not find-hash insert\n");
		hashInsert(hash_t, table_id, key);
		find = hashSearch(hash_t, table_id, key);
	}

	//����߰��� ��õ� �� �ٽ� ���нÿ��� fail
	if (!find) {
		pthread_mutex_unlock(&lock_table_latch);
		printf("lock acquire fail -1 \n");
		return NULL;
	}

	//�����Ѵٸ� �ش���ڿ� �� �߰�
	lock_t * tmp = find->head->next;
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

	//�̹� �ش� ���ڵ尡 �����ִٸ�, �� �ڽ��� ������ lock�� �ƴѰ�� sleep
	if (find->head->next != find->tail->pre) {
//printf("cond wait...please sleep\n");
	pthread_cond_wait(&new_lock->cond,&lock_table_latch);
	}
	
	//���ؽ� ���
//printf("lock acquire : mutex unlock - finish\n");
	pthread_mutex_unlock(&lock_table_latch);
	return new_lock;
}

int lock_release(lock_t* lock_obj)
{
	if(lock_obj==NULL)return FAIL;
//printf("\nlock release start --- mutex lock\n");
	pthread_mutex_lock(&lock_table_latch);
	//�� ����Ʈ���� �������ش�
	//��ũ�� ����Ʈ ����
//printf("lock release : lock list delete\n");
	lock_obj->pre->next = lock_obj->next;
	lock_obj->next->pre = lock_obj->pre;

	//�����ڰ� �ִٸ� �����ش�.
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
printf("create error : memeory allocation fail -2\n");
		return NULL;
	}

	//�� ���̺������Ϳ� hashnode���� �Ҵ� �� �� �ʱ�ȭ
	for (int i = 0; i < size; i++)
	{
		hash_t->table[i] = (HashNode *)malloc(sizeof(struct HashNode));
		if (!hash_t->table[i]) {
printf("create error : memeory allocation fail -3\n");
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

	//���� �� ��帮��Ʈ���
	if (!tmp) {
//printf("search null : empty node list\n");
		return NULL;
	}

	//Ž�� ����
	while (tmp) {
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

	//���ؽ� �ʿ�� �Լ� ȣ��
	
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


