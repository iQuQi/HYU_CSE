
#ifndef __FILE_H__
#define __FILE_H__

#include "bpt.h"
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

//8����Ʈ ũ���� ����������

typedef struct page_t { // 4096Byte
	// in-memory page struccture
	pagenum_t parent;
	int is_leaf;
	int num_key;
	char reserved[104];
	pagenum_t right_left;//leaf�� ��� : ����������  / internal�� ��� : ����
	union {
		key_child branch[248]; //  key8+offset8 - 248��
		key_val record[31]; // key8+record120 - 31�� 
	};
}page_t;



// Allocate an on-disk page from the free page list
pagenum_t file_alloc_page();
// Free an on-disk page to the free page list
void file_free_page(pagenum_t pagenum);
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(pagenum_t pagenum, page_t* dest);
// Write an in-memory page(src) to the on-disk page
void file_write_page(pagenum_t pagenum, const page_t* src);

#endif
