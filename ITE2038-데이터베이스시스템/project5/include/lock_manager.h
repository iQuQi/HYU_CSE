#ifndef __LOCK_MANAGER_H__
#define __LOCK_MANAGER_H__

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>


#define FAIL -1
#define SUCCESS 0
#define LOAD_FACTOR 20

typedef struct lock_t lock_t;
typedef struct Node Node;
typedef struct HashNode HashNode;
typedef struct HashTable HashTable;

typedef struct lock_t {

	lock_t *pre;
	lock_t *next;
	pthread_cond_t cond; // �̰� ���߿� �ּ�Ǯ��
	Node * node_ptr; // �̰� ���õȰ� ����
	int lock_mode;
	lock_t *trx_next_lock;
	int owner_txn_id;
	//����� Ž���� ���� ��ȸ ����  -> Abort=�ش� txn�� lock���� release 
	//-> update/find �Լ������� return���� abort�Ϸ� -> 
	//������ ���ڵ� undo & lock ������Ʈ �ع� & �ش�Txn �� ���̺��� ����

	int sleep;
	char old_val[120];
	int change;
	int recovery_p;
	int recovery_r;

}lock_t;

typedef struct Node {
	int table_id;
	int64_t record_id;//key

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


HashTable * createHashTable(int size); 
Node* hashSearch(HashTable * hash_t, int table_id,int64_t r_id);
int hashInsert(HashTable * hash_t, int table_id,int64_t r_id);
int hashPrint(HashTable * hash_t);
int getKey(int64_t r_id,int table_size);


/* APIs for lock table */
int init_lock_table();//�굵 ���� ����
int lock_release(lock_t* lock_obj);//�굵 ����

lock_t* lock_acquire(int table_id, int64_t key,int trx_id, int lock_mode);//����߰� �� trxid ���� ���õȰ� ����
int lock_abort(lock_t * lock);//�굵
void show_lock_list(int64_t key,int table_id);

void show_lock_list(int64_t key, int table_id);

pthread_mutex_t lock_table_latch;
HashTable * hash_t;

#endif 
