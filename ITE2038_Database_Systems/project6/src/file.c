

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

int ch;

//프리페이지에서 떼와서 disk페이지를 할당한다.
pagenum_t file_alloc_page(int table_id) {

	//printf("\nfile alloc page start\n");
		//헤더를 버퍼에서 읽어온다.
	int head = pageScan(table_id, 0);
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
	setDirty(head);

	//현재 그 파일에서 사용하고 있는 페이지 수
	pagenum_t page_use = b_head.page_num;
	//printf("first page_use=%ld\n",page_use);
		//헤더에서 프리페이지 첫 위치를 찾는다
	pagenum_t free_p = b_head.free_page;
	//printf("first free_page=%ld\n",b_head.free_page);
		//만약 현재 프리페이지가 없다면 새로 더 할당해준다 //10개 만들어 주기
	int tmp;
	if (free_p == 0) {
		//새페이지 하나 불러와서 free페이지리스트앞에 넣어준다.
		tmp = pageScan(table_id, page_use);
		//printf("new free!! tmp= %d\n",tmp);
		b_head.free_page = page_use;
		//printf("now firstfree = %ld",page_use);
				//오른쪽프리페이지 아직없음
		b_M.frameArray[tmp].frame_p.parent = 0;

		//1증가
		b_head.page_num++;
		page_use++;

		//set/clear
		setDirty(tmp);
		clearPin(tmp);
pthread_mutex_unlock(&b_M.frameArray[tmp].page_latch);
		for (int i = 1; i < 5; i++)
		{
			//printf("free make = i: %d\n",i);
						//새페이지 하나 불러와서 free페이지리스트앞에 넣어준다. 오른쪽 꺼랑도 다시 연결
			tmp = pageScan(table_id, page_use);
			//printf("new free!! tmp= %d\n",tmp);
			//printf("um.. past free_page = %ld\n",b_head.free_page);
			//printf("um..past parent=%ld",b_M.frameArray[tmp].frame_p.parent);
			b_M.frameArray[tmp].frame_p.parent = b_head.free_page;
			b_head.free_page = page_use;

			//printf("now firstfree = %ld\n",page_use);
			//printf("now firstfree_right = %ld\n",b_M.frameArray[tmp].frame_p.parent);

						//1증가
			b_head.page_num++;
			page_use++;

			//set/clear
			setDirty(tmp);
			clearPin(tmp);
pthread_mutex_unlock(&b_M.frameArray[tmp].page_latch);

		}
	}
	//pagenum_t tmp_free=b_head.free_page;


	//넘겨줄 빈 페이지를 가져와서 오른쪽친구를 프리페이지헤더로 만들고 줌
	int now_free = pageScan(table_id, b_head.free_page);
	b_head.free_page = b_M.frameArray[now_free].frame_p.parent;
pthread_mutex_unlock(&b_M.frameArray[now_free].page_latch);
	//printf("alloc after :");
	//headInfo(table_id);
	//bufInfo();
	//freeInfo(table_id);

	clearPin(head);
	return now_free;//새 페이지를 올려둔 프레임 인덱스 반환
}


void file_free_page(int table_id, pagenum_t pagenum) {

	//printf("\nfree page 1\n");
		//헤더를 버퍼에서 읽어온다.
	int head = pageScan(table_id, 0);

	setDirty(head);
	//printf("free before :");
	//headInfo(table_id);
	//freeInfo(table_id);
		//받아온 페이지를 프리 페이지 리스트에 넣어준다. 
		//나중에 make page할때 다 초기화 하므로 지금 초기화해서 넣어 줄필요는 x
	int do_free = pageScan(table_id, pagenum);
pthread_mutex_unlock(&b_M.frameArray[do_free].page_latch);

	//리스트 연결
	b_M.frameArray[do_free].frame_p.parent = b_head.free_page;
	b_head.free_page = pagenum;
pthread_mutex_unlock(&b_M.frameArray[head].page_latch);
	//Drop 하고 clear
	pageDrop(do_free);
	clearPin(do_free);
	clearPin(head);
	//printf("free after :");
	//headInfo(table_id);
	//freeInfo(table_id);

}

void file_read_page(int table_fd, pagenum_t pagenum, page_t* dest) {
	//printf("\nfile read\n");
	off_t off = lseek(table_fd, pagenum*PAGESIZE, SEEK_SET);
	//printf("fd : %d\n",table_fd);
	ch = read(table_fd, dest, PAGESIZE);

	if (fsync(table_fd) == -1) printf("read fsync() failed\n");

	//printf("file read end\n");
}

void file_write_page(int table_fd, pagenum_t pagenum, const page_t* src) {
	//printf("\nfile write\n");

	off_t off = lseek(table_fd, pagenum*PAGESIZE, SEEK_SET);
	ch = write(table_fd, src, PAGESIZE);


	if (fsync(table_fd) == -1)printf("write fsync fail\n");
	//printf("file write end\n");

}
