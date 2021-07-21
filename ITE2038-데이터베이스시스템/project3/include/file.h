
#ifndef __FILE_H__
#define __FILE_H__

#include "bpt.h"
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

//8바이트 크기의 정수형선언




// Allocate an on-disk page from the free page list
pagenum_t file_alloc_page(int table_id);
// Free an on-disk page to the free page list
void file_free_page(int table_id,pagenum_t pagenum);
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(int table_id,pagenum_t pagenum, page_t* dest);
// Write an in-memory page(src) to the on-disk page
void file_write_page(int table_id,pagenum_t pagenum, const page_t* src);

#endif
