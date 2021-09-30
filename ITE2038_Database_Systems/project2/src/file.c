

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

#include "file.h"
#include "bpt.h"
int ch;
int op_fd;
//프리페이지에서 떼와서 disk페이지를 할당한다.

pagenum_t file_alloc_page() {
	page_t * p;
	header_page * header;
	p = (page_t *)calloc(1, PAGESIZE);
	header = (header_page *)calloc(1, PAGESIZE);
	file_read_page(0, (page_t *)header);

	//헤더에 0 offset을 읽어 와서 저장한다.


	//헤더에서 프리페이지 첫 위치를 찾는다
	pagenum_t free_p = header->free_page;


	//만약 현재 프리페이지가 3개보다 작다면 새로 더 할당해준다 -- 15개정도(for)
	//증가한만큼 free num 늘려주기
	if (free_p ==0) {

		p->parent =0;
		header->free_page=header->page_num;
		file_write_page(header->page_num, p);
		for (int i = 1; i < 15; i++)
		{	
			p->parent = header->free_page;
			
			
			//뒤로 하나씩 write 해주고 free list에서는 헤더다음으로 계속 끼워넣어준다. - 즉 header가 top쪽인 stack형태
			file_write_page(header->page_num + i, p);
			header->free_page = header->page_num + i;

		}

		header->page_num += 15;
		free_p = header->free_page;

	}

	//넘겨줄 빈 페이지를 가져와서 부모를 알아본다
	file_read_page(free_p,p);
	//free list에서 이페이지를 used list로 넣을 거니까 제외시키고 그 parent(next)를 첫 freepage로 한다.
	header->free_page = p->parent;


	file_write_page(0, (page_t *)header);
	//수정된헤더를 0 offset에 다시 써준다 
	
	free(header);
	free(p);
	//프리페이지 공간이 있었다면 첫번째 프리페이지num을 리턴

	
	return free_p;
}
	

void file_free_page(pagenum_t pagenum) {
	header_page * header=(header_page *)calloc(1,PAGESIZE);
//printf("free page func\n");
	file_read_page(0, (page_t *)header);
	//헤더에 0 offset을 읽어 와서 저장한다.

	//새로 페이지를 생성하고 next를 현재 처음프리페이지로 설정/헤더의 첫 프리페이지를 이 페이지로 갱신
	page_t * p;
	p = (page_t *)calloc(1, PAGESIZE);
	p->parent = header->free_page;
	header->free_page = pagenum;
	file_write_page(pagenum, p);
	//지울 페이지를 덮어씌워줘야함
	//페이지넘에 새로생성한 페이지를 write


	//used ++ free -- 이므로 전체 페이지 수는 그대로

	//갱신된 헤더를 다시 0번에 write
	file_write_page(0, (page_t *)header);

	free(header);
	free(p);

}

void file_read_page(pagenum_t pagenum, page_t* dest) {
	/*
	ssize_t read(int fd, void *buf, size_t nbytes)
	ssize_t pread(int fd, void *buf, size_t nbytes, off_t offset)
	off_t lseek (int fd, off_t offset, int whence )
		리터값 =>
	  성공 = 읽은 바이트수
	 0 =파일의 끝일경우
	  -1 = 오류가 발생 
     SEEK_SET: 파일의 시작점을 기준으로 이동
	 SEEK_CUR : 현재 위치를 기준으로 이동
	 SEEK_END : 파일의 끝을 기준으로 이동

	write =>dest : 쓸 데이터를 저장하고 있는 메모리의 시작 주소
	*/

	off_t off = lseek(op_fd,pagenum*PAGESIZE,SEEK_SET);
	ch = read(op_fd,dest,PAGESIZE);

	if (ch < PAGESIZE) {

		printf("read failed : %ld\n",pagenum);
		
	}
	
	if(fsync(op_fd)==-1){

	printf("read fsync() failed\n");
	}
	
}

void file_write_page(pagenum_t pagenum, const page_t* src) {
	//ssize_t pwrite(int fd, const void *buf, size_t nbytes, off_t offset);
	//성공시 기록한 바이트수,실패시 반환값 -1 
	//ssize_t write(int fd, void *buf, size_t nbytes);
	//off_t lseek(int fd, off_t offset, int whence)

	//read : src=읽은 데이터를 저장할 메모리의 시작주소

	off_t off= lseek(op_fd,pagenum*PAGESIZE,SEEK_SET);
	ch=write(op_fd,src,PAGESIZE);
 

	if (ch ==-1) {

		printf("write failed\n");
		
	}

	if(fsync(op_fd)==-1){
		printf("write fsync fail\n");
}
	
}
