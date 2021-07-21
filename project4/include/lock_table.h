#ifndef __LOCK_TABLE_H__
#define __LOCK_TABLE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>


#define FAIL -1
#define SUCCESS 0


typedef struct lock_t lock_t;
typedef struct Node Node;
typedef struct HashNode HashNode;
typedef struct HashTable HashTable;




/* APIs for lock table */
int init_lock_table();
lock_t* lock_acquire(int table_id, int64_t key);
int lock_release(lock_t* lock_obj);

//hash funct
HashTable * createHashTable(int size); 


Node* hashSearch(HashTable * hash_t, int table_id,int64_t r_id);
int hashInsert(HashTable * hash_t, int table_id,int64_t r_id);
//int hashDelete(HashTable * hash_t, int table_id,int64_t r_id);
int hashPrint(HashTable * hash_t);
int getKey(int64_t r_id,int table_size);


pthread_mutex_t lock_table_latch;

HashTable * hash_t;

#endif /* __LOCK_TABLE_H__ */
