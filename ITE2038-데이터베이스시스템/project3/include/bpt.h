

#ifndef __BPT_H__
#define __BPT_H__

#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<inttypes.h>//PRid64 - int64_t를 정수로 출력
#include<sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>


#define  PAGESIZE 4096
////////////
#define FAIL -1
#define SUCCESS 0 
#define HEAD - 2
#define TAIL -3
/////////////
#define leaf_order 31
#define node_order 248
////////////
#define b_index b_M.frameArray[index]
#define b_page b_M.frameArray[index].frame_p
////////////
#define b_head b_M.frameArray[head].frame_h
#define b_root b_M.frameArray[root].frame_p
////////////////////
#define b_leaf b_M.frameArray[leaf].frame_p
#define b_parent b_M.frameArray[parent].frame_p
#define b_child b_M.frameArray[child].frame_p
#define b_nei b_M.frameArray[nei].frame_p
#define b_right b_M.frameArray[right].frame_p
#define b_left b_M.frameArray[left].frame_p
#define b_page_parent b_M.frameArray[page_parent].frame_p
#define b_old b_M.frameArray[old_p].frame_p
#define b_new b_M.frameArray[new_p].frame_p

extern int ch;
extern int op_fd;
typedef uint64_t pagenum_t;
typedef struct queue {
	int count;
	int pnum;
	struct queue* next;
}queue;


typedef struct record {
	int64_t key;//64bit 정수 
	char val[120];
} key_val;


typedef struct key_child {
	int64_t key; //8바이트
	pagenum_t child;
} key_child;


typedef struct page_t { // 4096Byte
	// in-memory page struccture
	pagenum_t parent;
	int is_leaf;
	int num_key;
	char reserved[104];
	pagenum_t right_left;//leaf의 경우 : 오른쪽형제  / internal의 경우 : 왼쪽
	union {
		key_child branch[248]; //  key8+offset8 - 248개
		key_val record[31]; // key8+record120 - 31개 
	};
}page_t;


typedef struct header_page {
	pagenum_t free_page;//[0-7] free page offset
	pagenum_t root_page;//[8-15] root page offset
	int64_t page_num;// [16-23] number of pages
	char reserved[4072];//reserved
} header_page;

typedef struct bufferStructure {
	union {
		header_page frame_h;
		page_t frame_p;
	};

	pagenum_t page_num;
	int table_id;

	int next;
	int pre;

	//bool refer;
	bool isdirty;
	bool ispinned;


}buffer_S;

typedef struct Table {
	int fd;
	int id;
	char * path;
	int isopen;
}Table;

typedef struct bufferManager {
	
	

	Table table[10];
	int table_use;
	int table_total;

	int frame_capacity; 
	int use_num;

	int LRU_head;
	int LRU_tail;
	buffer_S * frameArray;
	
}buffer_M;

buffer_M b_M;

//free page는 leaf/internal의 parent 영역을 같이 사용하도록 한다.
//헤더만 따로 구현해준다.



//ok
void print_tree(int table_id);
int showTable();
void enqueue(pagenum_t pagenum);
pagenum_t dequeue(void);
void menu();
void freeInfo(int tableid);
void headInfo(int tableid);
void bufInfo();

////////////////////////////////////
int db_insert(int table_id,int64_t key, char * value);
int db_find(int table_id,int64_t key, char * ret_val);//ok
int db_delete(int table_id, int64_t key);
////////////////////////////////////////



//find
int find_page(int tableid,int64_t key);//ok
int cut( int length );//ok

// Insertion.

int  make_internal_page( int tableid );//ok
int  make_leaf_page( int tableid );//ok

int get_left_index(int tableid,pagenum_t parent, pagenum_t left);//ok

int insert_into_new_root(int tableid,pagenum_t left, int64_t key, pagenum_t right);//ok
int start_new_tree(int tableid, int64_t key,char * val);//ok



//--------------------------
int insert_into_leaf(int tableid,pagenum_t l, int64_t key, char * val ); //ok
int insert_into_leaf_after_splitting(int tableid,pagenum_t l, int64_t key,char * val);
int insert_into_node( int tableid,pagenum_t n_p,int left_index, int64_t key, pagenum_t right);
int insert_into_node_after_splitting(int tableid,pagenum_t old,  int left_index, int64_t key, pagenum_t right);
int insert_into_parent( int tableid, pagenum_t left, int64_t key, pagenum_t right);
//--------------------오늘 이까지



// Deletion.

int get_neighbor_index(int tableid,pagenum_t n_p );
int adjust_root(int tableid,pagenum_t r);

int coalesce_nodes(int tableid,pagenum_t n, pagenum_t n_p,int neighbor_index, int64_t k_prime);
int redistribute_nodes( int tableid,pagenum_t n, pagenum_t n_p, int neighbor_index,int k_prime_index, int64_t k_prime);
int delete_entry(int tableid,pagenum_t n, int64_t key);
int remove_entry_from_node(int tableid,pagenum_t n, int64_t key);
//--------------이까지 내일하고 디버깅 돌리기


//여기부터 버퍼 매니저

int init_db(int buf_num);
int open_table(char *pathname);
int close_table(int table_id);
int shutdown_db();
////////////////////
int pageDrop(int index);
int pageScan(int table,pagenum_t pagenum);
int pageLoad(int index);

int pinCheck();

void setPage(int table_id,pagenum_t page_num,int index);
void setDirty(int index);
void setPin(int index);
void clearDirty(int index);
void clearPin(int index);





#endif
