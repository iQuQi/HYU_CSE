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
	pthread_cond_t cond; // 이거 나중에 주석풀기
	Node * node_ptr; // 이거 관련된거 수정
	int lock_mode;
	lock_t *trx_next_lock;
	int owner_txn_id;
	//데드락 탐지를 위한 순회 가능  -> Abort=해당 txn꺼 lock전부 release 
	//-> update/find 함수내에서 return전에 abort완료 -> 
	//수정된 레코드 undo & lock 오브젝트 해방 & 해당Txn 를 테이블에서 삭제

	int sleep;
	char old_val[120];
	int change;
	int recovery_p;
	int recovery_r;

}lock_t;

typedef struct Node {
	int table_id;
	int64_t record_id;//key

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


HashTable * createHashTable(int size); 
Node* hashSearch(HashTable * hash_t, int table_id,int64_t r_id);
int hashInsert(HashTable * hash_t, int table_id,int64_t r_id);
int hashPrint(HashTable * hash_t);
int getKey(int64_t r_id,int table_size);


/* APIs for lock table */
int init_lock_table();//얘도 조금 수정
int lock_release(lock_t* lock_obj);//얘도 수정

lock_t* lock_acquire(int table_id, int64_t key,int trx_id, int lock_mode);//모드추가 및 trxid 정보 관련된거 수정
int lock_abort(lock_t * lock);//얘도
void show_lock_list(int64_t key,int table_id);

void show_lock_list(int64_t key, int table_id);

pthread_mutex_t lock_table_latch;
HashTable * hash_t;

#endif 
