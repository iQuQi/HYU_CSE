

#ifndef __BPT_H__
#define __BPT_H__

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


#define  PAGESIZE 4096

#define leaf_order 31
#define node_order 248

extern int ch;
extern int op_fd;
typedef uint64_t pagenum_t;
typedef struct queue {
	int count;
	int pnum;
	struct queue* next;
}queue;



void print_tree();
void enqueue(pagenum_t pagenum);
pagenum_t dequeue(void);
void menu();


int db_insert(int64_t key, char * value);
int open_table(char *pathname);
void close_table(int fd);
int db_find(int64_t key, char * ret_val);
int db_delete(int64_t key);



typedef struct record {
	int64_t key;//64bit ���� 
	char val[120];
} key_val;


typedef struct key_child {
	int64_t key; //8����Ʈ
	pagenum_t child;
} key_child;

//free page�� leaf/internal�� parent ������ ���� ����ϵ��� �Ѵ�.
//����� ���� �������ش�.
typedef struct header_page {
	pagenum_t free_page;//[0-7] free page offset
	pagenum_t root_page;//[8-15] root page offset
	int64_t page_num;// [16-23] number of pages
	char reserved[4072];//reserved
} header_page;

//find
pagenum_t find_page(int64_t key);
int cut( int length );

// Insertion.

pagenum_t  make_internal_page( void );
pagenum_t  make_leaf_page( void );

int get_left_index(pagenum_t parent, pagenum_t left);

pagenum_t insert_into_new_root(pagenum_t left, int64_t key, pagenum_t right);
pagenum_t start_new_tree(int64_t key, char * val);



//--------------------------���� �̱���(+���ϸŴ����� bpt2 + find)
pagenum_t insert_into_leaf(pagenum_t l, int64_t key, char * val );
pagenum_t insert_into_leaf_after_splitting(pagenum_t l, int64_t key,char * val);

pagenum_t insert_into_node( pagenum_t n_p,int left_index, int64_t key, pagenum_t right);
pagenum_t insert_into_node_after_splitting(pagenum_t old,  int left_index, int64_t key, pagenum_t right);
pagenum_t insert_into_parent( pagenum_t left, int64_t key, pagenum_t right);
//--------------------�̱��� ������



// Deletion.

int get_neighbor_index(pagenum_t n_p );
pagenum_t adjust_root(pagenum_t r);

pagenum_t coalesce_nodes(pagenum_t n, pagenum_t n_p,int neighbor_index, int64_t k_prime);
pagenum_t redistribute_nodes( pagenum_t n, pagenum_t n_p, int neighbor_index,int k_prime_index, int64_t k_prime);
pagenum_t delete_entry(pagenum_t n, int64_t key);
pagenum_t remove_entry_from_node(pagenum_t n, int64_t key);
//--------------�̱��� �����

//�ݿ��ϳ� �׽�Ʈ�غ��� �����ϱ�

//����ϳ� ���Ͻ����ۼ�




#endif
