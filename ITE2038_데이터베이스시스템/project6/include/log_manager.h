
#ifndef __LOG_H__
#define __LOG_H__

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

int log_fd;

//이번 프로젝트는 end offset 사용

#define BEGIN 0
#define UPDATE 1
#define COMMIT 2
#define ROLLBACK 3
#define COMPENSATE 4

#define BCR_SIZE 28
#define UP_SIZE 288
#define COM_SIZE 296




typedef struct type_BCR {


	int64_t LSN;
	int64_t pre_LSN;
	int trx_id;
	int type;
	int log_size;

}type_BCR;

typedef struct type_update {

	
	int64_t LSN;
	int64_t pre_LSN;
	int trx_id;
	int type;

	int table_id;
	int64_t page_num;
	int off;
	int data_len;
	char old_image[120];
	char new_image[120];
	int log_size;

}type_update;

typedef struct type_compen {

	
	int64_t LSN;
	int64_t pre_LSN;
	int trx_id;
	int type;

	int table_id;
	int64_t page_num;
	int off;
	int data_len;
	char old_image[120];
	char new_image[120];
	int64_t next_undo_LSN;
	int log_size;

}type_compen;

typedef struct log_bufferStructure {
	union {
		type_BCR frame_BCR;
		type_update frame_update;
		type_compen frame_compensate;

	};

	int type;
	int next;
	int pre;


}log_buffer_S;

typedef struct log_bufferManager {

	int frame_capacity;
	int use_num;
	int log_now;	

	log_buffer_S * logRecords;
	int head;
	int tail;

}log_buffer_M;

log_buffer_M log_M;

int open_log(char * log_path);
int open_msg_log(char*log_msg_path);
int init_log_buf(int capacity);

int log_to_file_BCR(int trx_id, int log_fd, type_BCR * log);
int log_to_file_UP(int trx_id, int log_fd, type_update * log);
int log_to_file_COM(int trx_id, int log_fd, type_compen * log);

int read_log_file(int mode);

int log_BCR(int trx_id,int type);
int log_compensate(int trx_id, int table_id, int64_t page_num, int target_log,int record_off);
int log_update(int trx_id,int table_id,int64_t page_num,char* old_image,char* new_image,int record_off);



void recovery_redo(int log_num);
void recovery_undo(int log_num);
int analysis();

#endif
