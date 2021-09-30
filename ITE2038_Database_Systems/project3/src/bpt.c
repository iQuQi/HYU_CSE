	


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
#include <errno.h>
#include "file.h"
#include "bpt.h"
//함수 성공시 0리턴 실패시 -1 리턴

int ch;

void bufInfo(){
printf("\n<buffer info>\n");

printf("table use : %d\n",b_M.table_use);
printf("table total : %d\n",b_M.table_total);
printf("use num : %d\n",b_M.use_num);
printf("LRU head : %d\n",b_M.LRU_head);
printf("LRU tail : %d\n",b_M.LRU_tail);


for(int i=0;i<b_M.frame_capacity;i++){

printf("%d : tableid,pagenum = (%d,%ld), next = %d, pre = %d \n",i,b_M.frameArray[i].table_id,b_M.frameArray[i].page_num,
b_M.frameArray[i].next,b_M.frameArray[i].pre);

printf("ispinned=%d, isdirty=%d\n",b_M.frameArray[i].ispinned,b_M.frameArray[i].isdirty);

if(b_M.frameArray[i].page_num!=0){
printf("parent = %ld / is leaf = %d / num key = %d / right_left = %ld\n",
 b_M.frameArray[i].frame_p.parent,b_M.frameArray[i].frame_p.is_leaf,
b_M.frameArray[i].frame_p.num_key,b_M.frameArray[i].frame_p.right_left);

}
else if(b_M.frameArray[i].table_id!=0){
printf("it is head\n");
}
else printf("not used\n");

printf("\n");
}


}

void freeInfo(int table_id){

printf("\n<free page> \n"); 
	int head = pageScan(table_id,0);
	int free = pageScan(table_id, b_head.free_page);
	

	int cnt=0;
	pagenum_t tmp_next=b_head.free_page;
	if(b_head.free_page==0) printf("no free page\n");
	else{
	while(1){
		if(cnt>20) break;
		cnt++;
		printf("free %d : pagenum = %ld, right=%ld \n",cnt,tmp_next,
		b_M.frameArray[free].frame_p.parent);
		tmp_next = b_M.frameArray[free].frame_p.parent;
		if(tmp_next==0)break;
		clearPin(free);
		free = pageScan(table_id, tmp_next);

	}
	}
	
	clearPin(free);
	clearPin(head);
}


void headInfo(int tableid){
	int head=pageScan(tableid,0);
	printf("\n<header info>\n");
	printf("root , free, pagenum = %ld, %ld, %ld\n",b_head.root_page,b_head.free_page,b_head.page_num);
	setDirty(head);
	clearPin(head);
	
}

int close_table(int table_id){
//printf("\nclosetable1 -- frame capacity : %d\n",b_M.frame_capacity);
	if(table_id>10 ||table_id<1) {
//printf("wrong close\n");
		return FAIL;
	}

	//그 테이블의 페이지들을 전부 디스크로 내려준다.
	for (int i = 0; i < b_M.frame_capacity; i++)
	{
//printf("closetable2 -- cmp > framArray[i].table id / close id = %d/%d\n",b_M.frameArray[i].table_id , table_id);
		
		if (b_M.frameArray[i].table_id == table_id) {
		b_M.frameArray[i].ispinned=0;
 		pageDrop(i);
		}
	}

	//테이블 배열에서 없애준다
	for (int i = 0; i <10; i++)
	{
//printf("closetable3\n");
		if (b_M.table[i].id == table_id) {b_M.table[i].isopen = 0;
		b_M.table_use--;
		close(b_M.table[i].fd);
		}
		
	}

	
	return SUCCESS;


}

int shutdown_db()
{
	//테이블에 올라와있는 개수만큼 close table
	for (int i = 0; i <10; i++)
	{
		close_table(b_M.table[i].id);
	}

	return SUCCESS;
}

////////////////////////////////////////////

int init_db(int buf_num)
{
//printf("\ninit db 1\n");
	//동적할당 시켜준다, calloc은 모두 0을 초기화... 그래도 혹시 모르니ㅠ
	b_M.frameArray = (buffer_S*)calloc(buf_num, sizeof(struct bufferStructure));
	for (int i = 0; i < 10; i++)
	{
		b_M.table[i].fd = 0;
		b_M.table[i].id = 0;
		b_M.table[i].path = (char*)calloc(20, sizeof(char));
		b_M.table[i].isopen = 0;
		
		
	}
	b_M.table_total = 0;
	b_M.table_use = 0;
	b_M.frame_capacity = buf_num;
	b_M.use_num = 0;

	b_M.LRU_head = -1;
	b_M.LRU_tail = -1;
	

	for (int i = 0; i < buf_num; i++)
	{
		b_M.frameArray[i].pre = -1;
		b_M.frameArray[i].next= -1;

	}

	return SUCCESS;
}



int open_table(char *pathname) {

	//헤더를 읽어온다.
	int root;
	int head;
	//O_creat 모드에서는 세번째 인자에 파일에 접근권한 설정가능
	//파일이 없는 경우 새로 생성해주는데 파일이 존재한다면 에러를 리턴(O_EXCL)

	
	//같은 파일명이 존재하는지 체크
	int i;
	for (i = 0; i < 10; i++)
	{
		if (!strcmp(pathname, b_M.table[i].path)) {
			break;
		}
	}
	
//printf("open table : i= %d\n",i);
	//이전에 열린적이 없던 테이블
	if (i == 10) {
		if (b_M.table_total == 10) {
			printf("full table\n");
			return FAIL;
		}

//printf("open table : first open-1\n");
		i = b_M.table_total;
		b_M.table[i].fd= open(pathname, O_RDWR | O_CREAT | O_EXCL, 0777);
		int table_fd = b_M.table[i].fd;
		int table_id ;
		//정상적으로 열렸다면
		if (table_fd > 0) {
//printf("open table : first open-2\n");

			b_M.table[i].id = i + 1;
			table_id = b_M.table[i].id;
			strcpy(b_M.table[i].path, pathname);
			b_M.table[i].isopen = 1;
			b_M.table_total++;
			b_M.table_use++;

//printf("head scan : %d\n",table_id);
			head = pageScan(table_id, 0);
			//헤더 페이지 개수 카운트
			b_head.page_num = 1;
			b_head.free_page = 0;
			b_head.root_page = 0;

			//헤더 dirty/pin set
			clearPin(head);
			setDirty(head);

			return table_id;
		}

		//생성되어있다면 다시 읽어온다.
		b_M.table[i].fd = open(pathname, O_RDWR);
		table_fd = b_M.table[i].fd;

		//정상으로 읽어진경우
		if (table_fd > 0) {
			
//printf("open table : first open-3\n");
			b_M.table[i].id = i + 1;
			table_id = b_M.table[i].id;
	  		head = pageScan(table_id, 0);

			strcpy(b_M.table[i].path, pathname);
			b_M.table[i].isopen = 1;
			b_M.table_total++;
			b_M.table_use++;
//printf("open table : first open-4\n");
			//루트페이지를 버퍼에 올려준다
			if (b_head.root_page != 0) {
//printf("open table : first open-5\n");
				root = pageScan(table_id, b_head.root_page);
				clearPin(root);
			}
//printf("open table : first open-6\n");
			clearPin(head);

			return table_id;
		}

		
	}

	//이전에 열린적이 있어 정보가 저장되어있는 경우 fd만 갱신해서 저장
	else {
//printf("open table : already open-1\n");
		b_M.table[i].fd = open(pathname, O_RDWR);
//printf("new fd : %d\n",b_M.table[i].fd);
		int table_fd = b_M.table[i].fd;
		int table_id = b_M.table[i].id;

		if (table_fd > 0) {
//printf("open table : already open-2\n");
			//헤더를 버퍼에 올려준다
			head = pageScan(table_id, 0);
			b_M.table[i].isopen = 1;
			b_M.table_use++;

			//루트페이지를 버퍼에 올려준다
			if (b_head.root_page != 0) {
				root = pageScan(table_id, b_head.root_page);
				clearPin(root);
			}
			
			clearPin(head);

			return table_id;
		}

		
	}

//printf("open table : fail open\n");
	printf("open failed\n");
	return FAIL;
}





//////////////////////////////

int pageDrop(int index)
{
//printf("\npagedrop start : victim=%d \n",index);
	//모두 사용중인지 확인
	if (pinCheck() == FAIL) {
		return FAIL;
	}

	//링크드 리스트에서 삭제
	int pre = b_index.pre;
	int next = b_index.next;

	//drop할 페이지가 헤드였다면
	if (pre == HEAD) {
//printf("pagedrop head \n");
		if(next!=TAIL){
		b_M.LRU_head = b_index.next;
		b_M.frameArray[b_index.next].pre = HEAD;
		}
		else {
		b_M.LRU_head=-1;
		}
	}
	//테일 이었다면
	else if (next == TAIL) {
//printf("pagedrop tail \n");
		b_M.LRU_tail = b_index.pre;
		b_M.frameArray[b_index.pre].next = TAIL;
	}
	//둘다 아닌경우
	else{
//printf("pagedrop normal \n");
		b_M.frameArray[pre].next = next;
		b_M.frameArray[next].pre = pre;
	}
	
	//초기화
	b_index.pre = -1;
	b_index.next = -1;

	//더티인경우 디스크에 써주고 없애준다
	if (b_index.isdirty) {
//printf("pagedrop dirty \n");
//printf("parent : %ld\n",b_page.parent);
		file_write_page(b_M.table[b_index.table_id-1].fd, b_index.page_num, &b_page);
	}
	b_index.table_id = 0;
	b_index.page_num = 0;

	//초기화
	b_M.use_num--;
	b_index.isdirty = 0;
	b_index.ispinned = 0;
//printf("pagedrop end \n");

//bufInfo();
	return SUCCESS;
}

int pageScan(int table,pagenum_t pagenum)
{
//printf("page scan : pagenum = %ld\n",pagenum);
	int victim;
	//원하는 페이지가 버퍼위에 올라가있는지 확인
	//있으면 pick
	//없으면 디스크에서 불러서 올려줘야함
	//불러준후 버퍼매니저 수정

	for (int i = 0; i < b_M.frame_capacity; i++)
	{
		if (b_M.frameArray[i].table_id == table && b_M.frameArray[i].page_num == pagenum){
//printf("scan_already exist : i=%d\n",i);
			setPin(i);
			return i;
		}
	}


	//빈자리가 있을경우 버퍼에 올려준다.
	if (b_M.frame_capacity > b_M.use_num) {

		for (int i = 0; i < b_M.frame_capacity; i++)
		{
			//사용중이지 않은 프레임을 찾으면 거기로 페이지를 가져온다
			if (b_M.frameArray[i].pre == -1 && b_M.frameArray[i].next == -1) {
				//file_read
//printf("scan_space exist : pagenum=%ld,i=%d\n",pagenum,i);
				setPage(table, pagenum, i);
				pageLoad(i);
				setPin(i);

				return i;
			}
		}
	}
	
	//빈자리가 없는경우
	else {
//printf("scan_space full\n");
		//모두 사용중인경우
		if (pinCheck() == FAIL) return FAIL;

//printf("victim1 : %d\n",b_M.LRU_tail);
		victim = b_M.LRU_tail;
		if (b_M.frameArray[victim].ispinned||victim==0) {

			while (1) {
				victim = b_M.frameArray[victim].pre;
				if (!b_M.frameArray[victim].ispinned&&victim!=0) break;
			}
//printf("victim change : %d\n",victim);
		}
		
		//용량이꽉찬경우 drop
		pageDrop(victim);
//printf("scanend\n");


	}
	

//printf("final victim : %d, pagenum : %ld\n",victim,pagenum);
	//비운 프레임에 새 페이지를 읽어온후 정보 갱신
	setPage(table, pagenum, victim);//read해서 tableid, pagenum 수정
	setDirty(victim);
	setPin(victim);

	pageLoad(victim);
//printf("pagescan end\n");
	return victim;
}

//새 페이지를 버퍼에 실었기 때문에 next/pre값을 갱신
int pageLoad(int index)
{
//printf("\npage load : index=%d \n",index);
	b_M.use_num++;

	if (b_M.use_num == 1) {
//printf("page load only use1_return\n");
		b_M.LRU_head = index;
		b_M.LRU_tail = index;
		b_index.pre = HEAD; // 자신이 맨앞
		b_index.next = TAIL; //자신이 맨끝

		return SUCCESS;
	}
//printf("page load more than one..\n");
	//새로 실은 페이지
	b_index.pre = HEAD; // 자신이 맨앞이라는 소리
	b_index.next = b_M.LRU_head;

	//앞뒤로 수정
	b_M.frameArray[b_M.LRU_head].pre = index;
	b_M.LRU_head = index;
	

//bufInfo();
//printf("pageload end\n");
	return SUCCESS;
}


//모든 프레임이 사용중인지 체크하는 함수
int pinCheck()
{
	for (int i = 1;i<b_M.frame_capacity ; i ++)
	{

		if (b_M.frameArray[i].ispinned==0) {
//printf("pinCheck... : i=%d\n",i);
			return SUCCESS;
		}
		
	}
	printf("all page is pinnded. Please wait and try again....\n");
	//실패
	return FAIL;
	
}


//페이지를 읽어와서 업데이트
void setPage(int table_id,pagenum_t page_num,int index)
{

//printf("\nsetpage start \n");

//bufInfo();
	file_read_page(b_M.table[table_id-1].fd, page_num, &b_page);
	b_index.table_id = table_id;
	b_index.page_num = page_num;
//printf("setpage end\n");
//bufInfo();
	
}

//아래는 전부 set/clear
void setDirty(int index)
{
	b_index.isdirty =1;
}

void setPin(int index)
{
	b_index.ispinned = 1;
}

void clearDirty(int index)
{
	b_index.isdirty = 0;
}

void clearPin(int index)
{
	b_index.ispinned = 0;
}










/////////








int db_insert(int tableid,int64_t key, char * value) {
	//printf("db insert 1\n");
	//헤더를 받아온다.
	if(b_M.table[tableid-1].isopen==0) {
		printf("File Is Closed\n");
		return FAIL;
	}
//printf("\ndb_insert - 1\n");
	int head = pageScan(tableid, 0);
//printf("db_insert - head find success\n");
	//찾은 경우 -- 이미 있는 경우

//find case--- duplicate
	if (!db_find(tableid,key, value)) {
//printf("db_insert - duplicate\n");
		clearPin(head);
		return FAIL;
	}

	//못찾은경우 -- 넣을 수 있음
	//트리가 안만들어져 있는경우(첫 삽입인 경우)
	if (b_head.root_page == 0) {
//printf("db_insert - start new tree\n");
		clearPin(head);
		return start_new_tree(tableid,key, value);
	}

	//트리가 만들어져있는 경우
	//키가 들어갈 리프페이지 읽어들인다
	int leaf = find_page(tableid,key);
	pagenum_t l = b_M.frameArray[leaf].page_num;
//printf("db_insert - leaf find success\n");
	//printf("db insert here -- leaf num = %ld\n",leaf);

	//리프 페이지에 남은 자리가 있는 경우
	if (b_leaf.num_key < leaf_order) {
//printf("db_insert - insert into leaf\n");
//printf("num key = %d and leaf order = %d\n",b_leaf.num_key,leaf_order);
		insert_into_leaf(tableid,l, key, value);
		clearPin(head);
		clearPin(leaf);
		return SUCCESS;
	}

	//남은 자리가 없다면 쪼개야함
//printf("db_insert - insert into after splitting\n");
	insert_into_leaf_after_splitting(tableid,l, key, value);
	clearPin(head);
	clearPin(leaf);
	return SUCCESS;

	//그 키가 이미 있는지 확인한다.
	//없다면 새 레코드를 생성한다
	//만약 루트노드도 없다면 그 키로 루트를 새로 만들어준다
	//그 키가 들어갈 페이지를 가져온다
	//insert into leaf시전
	//리턴으로 leaf after split호출

}

int db_find(int tableid,int64_t key, char * ret_val) {
	int i;
	if(b_M.table[tableid-1].isopen==0) {
		printf("File Is Closed\n");
		return FAIL;
	}
//printf("\ndb_find-1\n");
	//그 키가 들어있을수도 있는 리프페이지를 가져온다
	int find_p = find_page(tableid,key);
//printf("db_find --- find page success\n");
	//빈트리인 경우
	if (find_p == FAIL) {
//printf("db_find- empty\n");
		clearPin(find_p);
		return FAIL;
	}

	//키를 발견하면 멈춘다
	for (i = 0; i < b_M.frameArray[find_p].frame_p.num_key; i++)
	{
		if (b_M.frameArray[find_p].frame_p.record[i].key == key) break;
	}
//printf("db_find-2\n");
	//인덱스가 끝번이 아니라면 찾지못한것 - break로 멈춰진게 아니라는 뜻
	if (i == b_M.frameArray[find_p].frame_p.num_key) {
//printf("db_find----- not find\n");
		clearPin(find_p);
		return FAIL;
	}
	else {
//printf("db_find------ find\n");
		strcpy(ret_val, b_M.frameArray[find_p].frame_p.record[i].val);
		clearPin(find_p);
		return SUCCESS;
	}
	//만약 받아온 페이지가 null이라면 리턴종료
	//있다면 그 페이지 내에서 key와 일치하는 것을 찾는다
	//for 돌린후 키의 인덱스를 찾는다 i 5 
	//그 인덱스가 numkey와 같다면 못찾은것 --- -1리턴
	//아니라면 val를 저장하고  0이런
}

int db_delete(int tableid,int64_t key) {
//printf("\ndb_delete\n");
	if(b_M.table[tableid-1].isopen==0) {
		printf("File Is Closed\n");
		return FAIL;
	}
	char * tmp = (char *)malloc(120);
	int leaf = find_page(tableid,key);
	pagenum_t l = b_M.frameArray[leaf].page_num;

	//빈트리
	if (leaf==FAIL) {
//printf("empty\n");
		clearPin(leaf);
		return FAIL;
	}

	int ch = db_find(tableid,key, tmp);
	//찾았다면
	if (ch == 0) {
//printf("delete entry call\n");
		delete_entry(tableid,l, key);
		free(tmp);
		return SUCCESS;
	}

	//못찾았다면 지울수없음
	else {
//printf("not find\n");
		free(tmp);
		return FAIL;
	}

}

///////////////
//나중에 지울부분
queue * q;
void print_tree(int table_id) {
	printf("\n<print tree>\n");
	pagenum_t nnum, rnum;

	//헤더받아오기
	int head = pageScan(table_id, 0);

	//if  no root
	if (b_head.root_page == 0) {
		printf("empty tree.\n");
		clearPin(head);
		return;
	}

	q = (queue*)malloc(sizeof(queue));
	q = NULL;
//printf("current root = %ld",b_head.root_page);
	rnum = b_head.root_page;
	enqueue(rnum);
	while (q != NULL) {
		//dequeue and trace
		nnum = dequeue();
		int tmp = pageScan(table_id, nnum);
	
		printf("<my parent = %ld> <my pagenum = %ld> ",b_M.frameArray[tmp].frame_p.parent,nnum);

		for (int i = 0; i < b_M.frameArray[tmp].frame_p.num_key; ++i) {
	
			printf("%" PRId64 " ", b_M.frameArray[tmp].frame_p.is_leaf ? b_M.frameArray[tmp].frame_p.record[i].key : b_M.frameArray[tmp].frame_p.branch[i].key);
		}
		//isnot leaf enqueue
		if (!b_M.frameArray[tmp].frame_p.is_leaf) {

			enqueue(b_M.frameArray[tmp].frame_p.right_left);
//printf("enqueue : %ld\n",b_M.frameArray[tmp].frame_p.right_left);
			for (int i = 0; i < b_M.frameArray[tmp].frame_p.num_key; ++i){
				enqueue(b_M.frameArray[tmp].frame_p.branch[i].child);
//printf("enqueue : %ld\n",b_M.frameArray[tmp].frame_p.branch[i].child);
			}
		}
		printf("| ");
		clearPin(tmp);
	}
	printf("\n");
	


	
	clearPin(head);

}

int showTable()
{
printf("\n=========<Table List>========\n");

	for (int i = 0; i < 10; i++)
	{
		printf("%d : pathname = %s , table_id = %d , fd = %d, isopen= = %d\n", i + 1, b_M.table[i].path,
			b_M.table[i].id, b_M.table[i].fd, b_M.table[i].isopen);
	}
printf("=============================\n");
	
	return SUCCESS;
}



void enqueue(pagenum_t pagenum) {
	queue * enq = (queue *)malloc(sizeof(queue));
	queue * tmp;
	enq->pnum = pagenum;
	enq->next = NULL;

	// q is empty
	if (q == NULL) q = enq;

	// q has page
	else {
		tmp = q;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = enq;
	}
}

pagenum_t dequeue() {
	queue * n = q;
	pagenum_t rt = n->pnum;
	q = q->next;
	free(n);
	return rt;
}

void menu() {
	//menu print funct
	printf("\n\n===please input instruction===\n");
	printf("1.insert(i <key> <val> <table id>)\n2.delete(d <key> <table id>)\n3.find(f <key> <table id>)\n4.print tree(t <table id>)\n5.quit(q) : auto save \n");
	printf("6.shutdown(s) : close all table\n7.close table(c <table id>)\n8.open new file(o <pathname>)\n");
	    printf("==============================\n");
}




int insert_into_new_root(int tableid ,pagenum_t l, int64_t key, pagenum_t r) {
	
//printf("\ninsert into new root\n");
	//새 리프페이지를 받아온다
	int root = make_internal_page(tableid);
	int head = pageScan(tableid, 0);
	int right = pageScan(tableid, r);
	int left = pageScan(tableid, l);



	b_head.root_page = b_M.frameArray[root].page_num;
//printf("\ninsert into new root 2 -- change rootpage=%ld\n",b_head.root_page);
	b_root.parent = 0;//루트 flag
	b_right.parent =b_head.root_page;
	b_left.parent = b_head.root_page;
	
	b_root.num_key++;
	b_root.right_left = l ;
	b_root.branch[0].key = key;
	b_root.branch[0].child = r;
	
	setDirty(root);
	setDirty(head);
	setDirty(right);
	setDirty(left);
	clearPin(root);
	clearPin(head);
	clearPin(right);
	clearPin(left);
	return root;

}


//처음 삽입
int start_new_tree(int tableid,int64_t key,char * val) {

//printf("\nstart new tree 1 \n");

	//새 리프페이지를 받아온다
	int leaf = make_leaf_page(tableid);
//printf("leaf : %d\n",leaf);
	int head = pageScan(tableid, 0);
//printf("start new tree 2 \n");
	//헤더 설정
	b_head.root_page =b_M.frameArray[leaf].page_num;

//printf("start new tree - root : %ld \n",b_head.root_page);
	//b_head.page_num++;

	//리프페이지 설정
	b_leaf.parent = 0;
	b_leaf.num_key = 1;
	b_leaf.record[0].key = key;

	//char *strcpy(char *dest, const char *src);
	//scr를 dest에 복사
	strcpy(b_leaf.record[0].val, val);

	//set and clear
	setDirty(head);
	setDirty(leaf);
	clearPin(leaf);
	clearPin(head);
	return SUCCESS;
}

int find_page(int tableid,int64_t key) {

//printf("\nfind page 1\n");
	int i = 0;
	//헤더를 받아온다
	int head = pageScan(tableid, 0);

	//빈트리 일경우
	if (b_head.root_page == 0) {
//printf("find page - empty\n");
		clearPin(head);
		return FAIL;
	}

	//루트정보저장
	int trace = pageScan(tableid, b_head.root_page);
	pagenum_t tmp=b_head.root_page;

	while (!b_M.frameArray[trace].frame_p.is_leaf) {
		while (i < b_M.frameArray[trace].frame_p.num_key) {
			if (key >= b_M.frameArray[trace].frame_p.branch[i].key) {
				if(i==(b_M.frameArray[trace].frame_p.num_key -1)){
				break;
				}
				i++;
			}
			else {
				i--;
				break;
			}

		}

		if (i != -1) {
//printf("find page - 2\n");
			tmp = b_M.frameArray[trace].frame_p.branch[i].child;
			trace = pageScan(tableid, tmp);
		}
		else if (i == -1) {
//printf("find page - 3\n");
			tmp = b_M.frameArray[trace].frame_p.right_left;
			trace = pageScan(tableid, tmp);
		}
	
	}
	//while 이 끝났다는건 리프 까지 도착
	//이때 반환되는 리프 페이지에는 실제 키가 들어있을 수도 있고 없을 수도 있다
	//예를 들어 9를 찾는 경우, 7이상 10미만의 키들이 들어있어야하는 페이지가 반환되어도
	//거기에 9가 있다는 보장은 x

	int find = pageScan(tableid, tmp);
//printf("find page - 4\n");
	clearPin(head);
	clearPin(trace);
	clearPin(find);
	return find;
}


int cut(int length) {
	if (length % 2 == 0)
		return length / 2;
	else
		return length / 2 + 1;
}


// INSERTION//////////////////////////////////////////////

//여기서는 헤더정보 변경x ---alloc을 하면서 프리페이지 수정완료/총개수는 변함x
int make_internal_page(int table_id) {

//printf("\nmake internal page1\n");
	//페이지의 변경할 정보
	//리프/인터널 - numkey, {key, },parent, right_left, isleaf
	//헤더 - 루트,프리,총개수
	int newpage = file_alloc_page(table_id);
//printf("new alloc internal pagenum : %ld\n",b_M.frameArray[newpage].page_num);
	
	//값초기화 
	b_M.frameArray[newpage].frame_p.is_leaf = 0;
	b_M.frameArray[newpage].frame_p.num_key = 0;

	//아직 부모가 결정 안남
	b_M.frameArray[newpage].frame_p.parent = 0;

	//자기밑의 제일 왼쪽 페이지도 결정x
	b_M.frameArray[newpage].frame_p.right_left = 0;

	//키+offset또한 아직 설정할필요x
	setDirty(newpage);
	clearPin(newpage);
	return newpage;
}


int make_leaf_page(int tableid) {
//printf("\nmake leaf page1\n");
	//새 페이지 갖고 옴
	int leaf = make_internal_page(tableid);
//printf("new alloc leaf pagenum : %ld\n",b_M.frameArray[leaf].page_num);

	//리프로 set
	b_M.frameArray[leaf].frame_p.is_leaf = 1;
	setDirty(leaf);
	clearPin(leaf);
	return leaf;
}
/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 삽입할 키 왼쪽에 있는 노드에 대한 부모 포인터의 인덱스를 찾기 위해 insert_into_parent에 사용되는 도우미 기능.
 */

int get_left_index(int tableid,pagenum_t p, pagenum_t l) {

	int i;
	int parent = pageScan(tableid, p);
	int left = pageScan(tableid, l);

	for (i = 0; i < b_parent.num_key; i++)
	{
		if(b_parent.branch[i].child==l){
			clearPin(parent);
			clearPin(left);
			return i;
		}
	}
	
	clearPin(parent);
	clearPin(left);
	return -1;

}


//////////////////////////////////////////
int insert_into_leaf(int tableid,pagenum_t l, int64_t key, char * val) {
//printf("\ninsert into leaf -- leaf pagenum : %ld\n",l);
	int leaf = pageScan(tableid, l);
//printf("insert into leaf : leaf = %d\n",leaf);
	int insertion_point=0;
	
	//삽입위치를 찾는다.
	while (insertion_point < b_leaf.num_key &&  b_leaf.record[insertion_point].key < key)
		insertion_point++;

//printf("insert into leaf : lnsert point = %d\n",insertion_point);
	//삽입위치 자리를 밀어서 비워준다
	for (int i = b_leaf.num_key; i > insertion_point; i--) {
		b_leaf.record[i].key = b_leaf.record[i - 1].key;
		strcpy(b_leaf.record[i].val, b_leaf.record[i - 1].val);
		
	}

	//그 비워둔 위치에 삽입
	b_leaf.record[insertion_point].key = key;
	strcpy(b_leaf.record[insertion_point].val, val);
//printf("insert into leaf---before numkey++ : %d\n",b_leaf.num_key);
	b_leaf.num_key++;
//printf("insert into leaf---after numkey++ : %d\n",b_leaf.num_key);

	//수정된 leaf 다시 write하고 free
	setDirty(leaf);
	clearPin(leaf);
	
	return SUCCESS;
}


int insert_into_leaf_after_splitting(int tableid,pagenum_t l, int64_t key, char * val) {
//printf("\ninsert into leaf after splitting 1\n");
	//원래 있던 리프 만들어주기
	int leaf = pageScan(tableid, l);
	int insertion_index, split, new_key, i, j;

	//임시공간 마련
	key_val* tmp_record = (key_val *)malloc(33*sizeof(struct record));

	//쪼개서 넣어줄 새 리프 만들어주기
	int new_p = make_leaf_page(tableid);
	pagenum_t n_p = b_M.frameArray[new_p].page_num;
//printf("insert into leaf after splitting 2 --- leaf pagenum = %ld\n",n_p);
	//tmp키포인터와 포인터를 가리키는 포인터를 만들어둔다 ? 왜 ---
	//그니까 원래3개만 들어갈수 있는데 하나가 더들어온경우 4개짜리를 보관할수있는 temp를 만들고
	//그걸 split해서 두개로 쪼개서 넣어준다는 말씀

	//삽입위치 찾기
	insertion_index = 0;
	while (insertion_index < leaf_order && b_leaf.record[insertion_index].key < key)
		insertion_index++;

//printf("insert into leaf after splitting 3 -- insert index = %d\n",insertion_index);
	//삽입위치인경우 하나띄워서 leat의 값을 복사해서 가져와서 저장 
	for (i = 0, j = 0; i < b_leaf.num_key; i++, j++) {
		if (j == insertion_index) j++;
		tmp_record[j].key = b_leaf.record[i].key;
		strcpy(tmp_record[j].val, b_leaf.record[i].val);
	}
	//빈 삽입위치에 넣어주기
	tmp_record[insertion_index].key = key;
	strcpy(tmp_record[insertion_index].val, val);


	//원래 리프를 비우고 다시 채워준다
	b_leaf. num_key= 0;

	//분할지점 정하기
	split = cut(leaf_order);
//printf("insert into leaf after splitting 4 -- split=%d\n",split);
	//원래 리프의 쪼갠부분의 앞쪽을 넣어주기
	for (i = 0; i < split; i++) {
		strcpy(b_leaf.record[i].val , tmp_record[i].val);
		b_leaf.record[i].key = tmp_record[i].key;
		b_leaf.num_key++;
	}
	//새 리프에 쪼갠부분의 나머지 부분을 넣어주기
	for (i = split, j = 0; i <= leaf_order; i++, j++) {
		strcpy(b_M.frameArray[new_p].frame_p.record[j].val , tmp_record[i].val);
		b_new.record[j].key = tmp_record[i].key;
		b_new.num_key++;
	}

	//tmp공간 free
	

	//새 리프랑 원래리프를 연결시켜준다  물론 그전에 원래리프가 가리키고 있던
	//다음 부분을 새로만든게 가리킬수 있도록 업뎃을 먼저 해둬야함
	b_new.right_left = b_leaf.right_left;
	b_leaf.right_left = n_p;

	//원래 리프를 재구성했으므로 원래값이 들어있던 부분들이 사라졌기때문에
	//거기에 대한 조정을 해준다
	for (i = b_leaf.num_key; i < leaf_order ; i++) {
		b_leaf.record[i].key = -1;
		
	}


	//값이 들은곳 뒤쪽부분 정리
	for (i = b_new.num_key; i < leaf_order ; i++) {
		b_new.record[i].key = -1;
		
	}

	//새로 만들어준 리프를 부모와 연결시켜주고 new키 설정 
	b_new.parent = b_leaf.parent;
	new_key = b_new.record[0].key;

	setDirty(leaf);
	setDirty(new_p);
	clearPin(leaf);
	clearPin(new_p);
	free(tmp_record);

	return insert_into_parent(tableid,l, new_key, n_p);
}


int insert_into_node(int tableid,pagenum_t n_p,int left_index, int64_t key, pagenum_t right) {
//printf("\ninsert into node 1 --- left index = %d\n",left_index);	
	int i;
	//n_p에 parent를 받아옴
	int page_parent = pageScan(tableid, n_p);

	//left 인덱스까지 한칸씩 오른쪽으로 미뤄서 자리를 확보한다
	for (i = b_page_parent.num_key; i > left_index; i--) {
		b_page_parent.branch[i + 1].child = b_page_parent.branch[i].child;
		b_page_parent.branch[i + 1].key = b_page_parent.branch[i].key;
	}

	//키와 포인터를 삽입
	b_page_parent.branch[left_index + 1].child = right;
	b_page_parent.branch[left_index+1].key = key;
	b_page_parent.num_key++;
	
	setDirty(page_parent);
	clearPin(page_parent);

//printf("insert into node end\n");
	return SUCCESS;
}

int insert_into_node_after_splitting(int tableid,pagenum_t old, int left_index, int64_t key, pagenum_t right) {

//printf("\ninsert into node after splitting 1\n");
	//into parent함수에서 old에 parent를 넘겨줌
	int i, j, split;
	int64_t k_prime;
	
	//원래 있던 노드 만들어주기
	int old_p = pageScan(tableid, old);

	//쪼개서 넣어줄 새 노드 만들어주기
	int new_p = make_internal_page(tableid);
	pagenum_t n_p = b_M.frameArray[new_p].page_num;
// printf("\ninsert into node after splitting 2 -- alloc complete : %d \n",new_p);
	/*
	 먼저 새 키와 포인터를 포함한 모든 것을 올바른 위치에
	 삽입할 수 있는 임시 키와 포인터를 만드십시오.
	 그런 다음 새 노드를 생성하고 키와 포인터의 절반은 이전 노드에,
	 나머지 절반은 새 노드에 복사하십시오.
	 */
	 //임시공간 마련
	key_child * tmp_key_child = (key_child *)malloc(250 * sizeof(struct key_child));

	//삽입할 위치를 비워준다
	for (i = 0, j = 0; i < b_old.num_key + 1; i++, j++) {
		if (j == left_index + 1) j++;
		tmp_key_child[j].child = b_old.branch[i].child;
	}
	for (i = 0, j = 0; i < b_old.num_key; i++, j++) {
		if (j == left_index+1) j++;
		tmp_key_child[j].key = b_old.branch[i].key;
	}

	//삽입할 위치에 삽입
	tmp_key_child[left_index + 1].child = right;
	tmp_key_child[left_index+1].key = key;

	/* Create the new node and copy
	 * half the keys and pointers to the
	 * old and half to the new.
	 새 노드를 생성하고 키와 포인터의 절반을 기존 노드에, 포인터 절반은 새 노드에 복사하십시오.
	 */
	 //쪼개는 위치 찾기
	split = cut(node_order);
	//원래 노드 비워주기
	b_old.num_key = 0;

	//원래있던 노드 채우기
	for (i = 0; i < split-1; i++) {
		b_old.num_key++;
		b_old.branch[i].key = tmp_key_child[i].key;
		b_old.branch[i].child= tmp_key_child[i].child;
		
	}

	//리프의 경우는 자신이 자식이 없기 때문에 그냥 올리는 작업을 했을때 문제x
	//그러나 인터널의 경우 자식을 갖고 올라가게 되므로
	//자신의 자식정보 수정이 필요


	k_prime = tmp_key_child[split-1].key;//올라갈 친구의 k_primed에 저장

	
	//새로만든 노드채우기
	b_new.num_key = 0;
	//split 지점 부터 넣음
	for (++i, j = 0; i <= node_order; i++, j++) {
		b_new.num_key++;
		b_new.branch[j].key = tmp_key_child[i].key;
		b_new.branch[j].child = tmp_key_child[i].child;
		
	}
	
	//올린 키가 원래 가리키고 있던 자식을 새로만든 노드의 right_left(rightmost)가 가리키게 설정
	b_new.right_left= tmp_key_child[split-1].child;

	//새로만든 노드 부모설정해주기
	b_new.parent = b_old.parent;

	//새로만든 노드의 자식의 부모설정도 마무리
	int child;
	for (i = 0; i < b_new.num_key; i++) {
//printf("\nchild changing...\n");
		child = pageScan(tableid, b_new.branch[i].child);
		b_child.parent = n_p;
		setDirty(child);
		clearPin(child);
		
	}
//printf("\nchild changing...\n");
	child = pageScan(tableid,b_new.right_left);
	b_child.parent =n_p;
	setDirty(child);

	setDirty(old_p);
	setDirty(new_p);
	clearPin(old_p);
	clearPin(new_p);
	clearPin(child);

	return insert_into_parent(tableid,old, k_prime, n_p);//올라갈 친구를 선정해서 부모에 삽입
}


int insert_into_parent(int tableid,pagenum_t l, int64_t key, pagenum_t right) {
//printf("\ninsert into parent 1\n");
	int left_index;
	pagenum_t p_page;

	int left = pageScan(tableid, l);
	p_page = b_left.parent;

//printf("insert into parent 2 : left=%d,parent page=%ld\n",left,p_page);

	//새루트에 넣어야하는 경우(즉 left가 루트(internal)였던 경우)
	if (p_page == 0) {
//printf("insert into new root call\n");
		clearPin(left);
		return insert_into_new_root(tableid,l, key, right);
	}

	//아닌경우 부모를 받아온다
	int parent = pageScan(tableid, p_page);
	//left가 부모쪽에서 몇번째 인덱스에 있는지 확인
//printf("insert into parent : parent = %ld\n",p_page);
	left_index = get_left_index(tableid,p_page, l);

	//부모페이지에 올려준다
	if (b_parent.num_key < node_order) {
//printf("insert into node call\n");
		clearPin(left);
		clearPin(parent);
		return insert_into_node(tableid,p_page, left_index, key, right);
	}
	 //부모로 하나 올렸는데 거기서도 꽉찬경우 다시 쪼개줘야함
	clearPin(left);
	clearPin(parent);
//printf("insert into node splitting call\n");
	return insert_into_node_after_splitting(tableid,p_page, left_index, key, right);
}





///////////////////////////////////////////////////////////////////////
// DELETION.////////////////////////////////////////////////////////
/*
삭제를 위한 유틸리티 기능. 노드의 가장 가까운 이웃(시블링) 인덱스가 있는 경우
 왼쪽을 검색하십시오.
 그렇지 않은 경우(노드는 가장 왼쪽의 하위 노드)이다. -1을 반환하여 이 특수 사례를 나타낸다.
 */

int get_neighbor_index(int tableid,pagenum_t n_p) {

	int new_p = pageScan(tableid, n_p);
	pagenum_t p = b_new.parent;
	int parent = pageScan(tableid, p);

	int i;

	/*
	키의 인덱스를 n_p을 가리키는 부모에 있는 포인터의 왼쪽인덱스으로 반환.
	 n이 가장 왼쪽의 아이일 경우, 이는 -2이 돌아온다는 것을 의미한다.
	 
	*/

	if (b_parent.right_left == n_p) {
		clearPin(new_p);
		clearPin(parent);
		return -2;
	}

	for (i = 0; i < b_parent.num_key; i++) {
		if (b_parent.branch[i].child == n_p) {
			//이때 i는 부모에게서 받아온 자기인덱스
			clearPin(new_p);
			clearPin(parent);
			return i - 1;//즉이건 부모쪽에서 확인한 key의 바로 왼쪽형제 인덱스
		}
	}

}

int remove_entry_from_node(int tableid, pagenum_t n, int64_t key) {
	int i;

//printf("\nremove entry from node start\n");
	int index = pageScan(tableid,n);

	// Remove the key and shift other keys accordingly.
	i = 0;
	//키위치 찾고 키를 당겨오면서 덮어씌워준다
	//internal인경우
	if(!b_page.is_leaf) {
//printf("\nremove entry from node : not leaf\n");
		while (b_page.branch[i].key != key)
			i++;
		//키랑 child  당겨주기
		for (++i; i < b_page.num_key; i++) {

			b_page.branch[i - 1].child= b_page.branch[i].child;
			b_page.branch[i - 1].key = b_page.branch[i].key;

		}
	}
	//리프인경우
	else if (b_page.is_leaf) {
//printf("\nremove entry from node : leaf\n");
		while (b_page.record[i].key != key)
			i++;

		//키랑 val 당겨주기
		for (++i; i < b_page.num_key; i++) {

			strcpy(b_page.record[i - 1].val, b_page.record[i].val);
			b_page.record[i - 1].key = b_page.record[i].key;

		}
	}



	// Remove the pointer and shift other pointers accordingly.
	// First determine number of pointers.

	// One key fewer.
	b_page.num_key--;

	// Set the other pointers to NULL for tidiness.//정돈을 위해

	if (b_page.is_leaf) {
//printf("\nremove entry from node : leaf-tidiness\n");

		for (i = b_page.num_key; i < leaf_order; i++) {
			b_page.record[i].key = -1;
			b_page.record[i].val[0] = '\0';
		}
			
	}
	else {
//printf("\nremove entry from node : not leaf-tidiness\n");
		for (i = b_page.num_key; i < node_order; i++) {
			b_page.branch[i].child = -1;
			b_page.branch[i].key = -1;
		}

	}

	setDirty(index);
	clearPin(index);
//printf("\nremove entry from node end\n");
	return n;
}


int adjust_root(int tableid, pagenum_t r) {
//printf("\nadjust root\n");
	//지운게 루트노드에 있을때의 조절과정임 - 이미 지우기는 끝난상태
	int root = pageScan(tableid, r);
	int new_root = pageScan(tableid, b_root.right_left);
	int head = pageScan(tableid, 0);

	/* Case: nonempty root.
	키랑 포인터가 이미 지워져있고 할게 없는 경우
	 * Key and pointer have already been deleted,
	 * so nothing to be done.
	 */
	 //비어있지 않은 루트 조절-- 그니까 이전에 삭제를 했는데 이후의 루트가 비어있지 않으면 별문제
	 //없으므로 그냥 끝이라는 소리
	if (b_root.num_key > 0) {
//printf("adjust root : not problem!\n");
		clearPin(head);
		clearPin(root);
		clearPin(new_root);
		return r;
	}

	/* Case: empty root. 그니까 이전에 지워서 넘겼더니 보니까 루트가 비어버린거임
	 그니까 아래자식이 있으면 루트로 올려줘야함
	 자식도 없엇으면 통째로 비어있는 트리가 된거임
	 */
	 //비어있는 루트인경우
	 // If it has a child, promote 
	 // the first (only) child
	 // as the new root.

	 //자녀가 있는 경우 첫 번째(only) 자식을 새 루트로 만든다
	 //루트가 리프가 아닐때
	if (!b_root.is_leaf) {
//printf("adjust root : not leaf\n");
		file_free_page(tableid,r);
		b_head.root_page = b_root.right_left;
		b_M.frameArray[new_root].frame_p.parent = 0;
		
	}
	//
	// If it is a leaf (has no children),
	// then the whole tree is empty.

	//루트가 리프일때 --- child가 없을때 즉이건 모든 트리가 비어있다는 것
	//루트페이지를 지우면됌
	else {
//printf("adjust root : leaf\n");
		file_free_page(tableid,r);
		b_head.root_page = 0;
	}

	setDirty(head);
	setDirty(root);
	setDirty(new_root);

	clearPin(head);
	clearPin(root);
	clearPin(new_root);
//printf("adjust root : complete!\n");
	//새로 조정된루트
	return new_root;
}

int coalesce_nodes(int tableid,pagenum_t n, pagenum_t n_p, int neighbor_index, int64_t k_prime) {
	//받아온 kprime은 지워질노드의 부모에서의 키
	//받아온 이웃 인덱스는 찐인덱스아님 -2 있음
	//n_p는 찐 이웃
//printf("\ncoal node\n");
	int i, j, neighbor_insertion_index, n_end, new_p, nei, t_p;
	pagenum_t tmp;

	pagenum_t change_neighbor;
	pagenum_t change_die;

	 //현재노드가 제일 왼쪽이면 오른쪽노드랑 합친다
	//대신 오른쪽 노드에 모이는게 아니고 여전히 왼쪽으로 모인다.
	//그리고 오른쪽 노드를 삭제하는 것임
	if (neighbor_index == -2) {

		//nei안에는 지워질 
		new_p = pageScan(tableid, n_p);
		nei = pageScan(tableid, n);
		change_neighbor = n;
		change_die=n_p;
	}
	else {

		new_p = pageScan(tableid, n);
		nei = pageScan(tableid, n_p);
		change_neighbor = n_p;
		change_die=n;
	}

	//부모페이지넘버를 받아온다.
	pagenum_t p = b_new.parent;

	/* 
	 n에서 키 및 포인터를 복사하기 위한 인접 노드의 시작점.
	 n과 이웃이 n이 가장 왼쪽의 자식인 경우에 자리를 바꾸었다는 것을 기억.
	 */

	neighbor_insertion_index = b_nei.num_key;

	 //리프가 아닌경우
	if (!b_new.is_leaf) {
//printf("coal node : not leaf\n");
		b_nei.branch[neighbor_insertion_index].key = k_prime;
		b_nei.branch[neighbor_insertion_index].child = b_new.right_left;
		b_nei.num_key++;
		
		//이웃노드로 옮겨주는 과정
		for (i = neighbor_insertion_index + 1, j = 0; j < b_new.num_key; i++, j++) {
			b_nei.branch[i].key = b_new.branch[j].key;
			b_nei.branch[i].child = b_new.branch[j].child;
			b_nei.num_key++;
			
		}

		//왼쪽으로 다 옮겨왔으므로 부모가 모두 동일할것임-- 부모수정은 x

		 //현재 n의 자식들도  neighbor를 가리키게 변경해줌
		for (i = 0; i < b_nei.num_key ; i++) {
			tmp = b_nei.branch[i].child;
			t_p = pageScan(tableid, tmp);
			b_M.frameArray[t_p].frame_p.parent =change_neighbor;
			setDirty(t_p);
			clearPin(t_p);
		}

		tmp = b_new.right_left;
		t_p = pageScan(tableid, tmp);
		b_M.frameArray[t_p].frame_p.parent = change_neighbor;
		setDirty(t_p);
		clearPin(t_p);
		file_free_page(tableid,change_die);
	
	}

	//리프인경우
	/*
	 key가 0개 이므로 바로free ,그리고 올바른 right page를 가리키게 재설정
	 */

	else {
//printf("\ncoal node : leaf\n");
		if(neighbor_index != -2){
//printf("\ncoal node : !=-2\n");
			b_nei.right_left = b_new.right_left;
			file_free_page(tableid,change_die);
		}
		else {//kprime is parent's index 0's key
//printf("\ncoal node : == -2\n");
			//이웃노드로 옮겨주는 과정
			for (i = neighbor_insertion_index, j = 0; j < b_new.num_key; i++, j++) {
	
				b_nei.record[i].key = b_new.record[j].key;
				strcpy(b_nei.record[i].val, b_new.record[j].val);
				b_nei.num_key++;
			
			}
			b_nei.right_left= b_new.right_left;
			file_free_page(tableid,change_die);
		
		}
		

		
	}

	//n의 흔적을 삭제시켜준다
	setDirty(nei);
//printf("coal : delete entry call\n");
	delete_entry(tableid,p, k_prime);
//printf("coal : delete entry return after call\n");
	clearPin(nei);
//printf("coal finish\n");
	return n;
}

int redistribute_nodes(int tableid,pagenum_t n, pagenum_t n_p, int neighbor_index, int k_prime_index, int64_t k_prime) {
//printf("\nredistribute node\n");

	//delay merge로인해  node는 절대 leaf일 수 x
	pagenum_t tmp;
	int i, t_p;
	int index = pageScan(tableid, n);
	int nei = pageScan(tableid, n_p);
	pagenum_t p = b_page.parent;
	int parent = pageScan(tableid, p);

	/* Case: n has a neighbor to the left.
		이웃의 마지막 키-점퍼 쌍을 이웃의 오른쪽 끝에서 n의 왼쪽 끝으로 당기십시오.
	그니까 이건 '이웃 - 나' 이렇게 있으면 이웃꺼 하나를 내꺼로 당겨오는거임
	즉 합치면 넘치니까 합치는 대신 빌려오는 거
	 */

	if (neighbor_index != -2) {
//printf("\nredistribute node\n");
		//리프가 아닌경우 
		//대신이때 리프가 아니라는건 자식도 있다는 소리니까 자식들도 옮겨주는 작업필요 - tmp
		if (!b_page.is_leaf) {
			//일단 앞에 한자리 비워야되니까 한칸씩 쭉밀어준다
//printf("\nredistribute node\n");

			//child포인터들을 이동
			b_page.branch[0].child = b_page.right_left;
			b_page.right_left = b_nei.branch[b_nei.num_key - 1].child;
			
			//빌려온 자식의 부모설정을 다시
			tmp = b_page.right_left;
			t_p = pageScan(tableid, tmp);
			b_M.frameArray[t_p].frame_p.parent = n;
			setDirty(t_p);

			//이웃의 키하나를 keyprime index에 부모로 올리고
			//그자리에서 부모의 key prime을 빌려온다(아이패드 그림 참조)
			b_page.branch[0].key = k_prime;
			b_parent.branch[k_prime_index].key = b_nei.branch[b_nei.num_key - 1].key;

			//이웃쪽은 뒤에 한칸을 초기화
			b_nei.branch[b_nei.num_key - 1].child = -1;
			b_nei.branch[b_nei.num_key - 1].key = -1;

			
		}

		//리프인경우 -  비워둔 자리에 옮겨준다
		//자식이 없으므로 신경쓸필요 없다
		else {

//printf("\nredistribute node\n");
			//원래라면 삭제후 최소크기를 만족못한 리프가 이웃으로 합쳐지고
			//합쳐졌을때 capacity를 초과하게되서 이함수가 불리는데 
			//이번의 경우 합쳐질 리프가 numkey가 0이므로 초과가 불가능
			//즉 이 함수 안에서는 leaf에 대한 고려는 하지 않아도 됨
		}
	}

	/* Case: n is the leftmost child.
	그니까  "나 - 이웃" 이렇게 있으면 이웃의 가장왼쪽앞요소를 내꺼 제일뒤쪽으로 당겨오는거임
	빌려온다고 생가 -> 이경우 내꺼 뒤에 넣는거 이므로 앞의 한칸을 마련할필요x 위의 경우보다 간단해짐
	 */
	 //아래의 key프라임은 이웃과 나사이의  부모쪽저장 키
	else {
//printf("\nredistribute node\n");
		//리프인경우 - 밑에 자식들에 대한 조정 필요x
		if (b_page.is_leaf) {

//printf("\nredistribute node\n");
			//원래라면 삭제후 최소크기를 만족못한 리프가 이웃으로 합쳐지고
			//합쳐졌을때 capacity를 초과하게되서 이함수가 불리는데 
			//이번의 경우 합쳐질 리프가 numkey가 0이므로 초과가 불가능
			//즉 이 함수 안에서는 leaf에 대한 고려는 하지 않아도 됨
		}

		//리프가 아닌경우 밑에자식들에 대한 조정도 필요
		else {
//printf("\nredistribute node\n");
			//부모쪽에서 keyprime을 갖고오고 이웃의 child pointer를 가져온다
			b_page.branch[0].key = k_prime;
			b_page.branch[0].child =b_nei.right_left;
			b_nei.right_left = b_nei.branch[0].child;
		
			tmp = b_page.branch[0].child;
			t_p = pageScan(tableid, tmp);
			b_M.frameArray[t_p].frame_p.parent = n;
			b_parent.branch[k_prime_index].key = b_nei.branch[0].key;
			setDirty(t_p);


			//이웃은 지금 앞의 한칸이 비워져있으므로 당겨와야함
			for (i = 0; i < b_nei.num_key-1; i++) {
				b_nei.branch[i].key = b_nei.branch[i + 1].key;
				b_nei.branch[i].child = b_nei.branch[i + 1].child;
			}
			b_nei.branch[i].key=-1;
			b_nei.branch[i].child=-1;
		}
	}


	/*
	 n은 이제 한 개의 열쇠와 한 개의 포인터를 더 가지고 있으며,
	 이웃은 각각 한 개씩 더 적게 가지고 있다.
	 그니까 하나 빌려왔다 이말
	 */

	b_page.num_key++;
	b_nei.num_key--;
//printf("\nredistribute node : set/clear\n");
	setDirty(index);
	setDirty(nei);
	setDirty(parent);
	clearPin(index);
	clearPin(nei);
	clearPin(parent);
	clearPin(t_p);
//printf("\nredistribute node : finish\n");
	return index;
}

int delete_entry(int tableid,pagenum_t n, int64_t key) {

	//받아온 페이지에서 그 키를 삭제
//printf("\ndelete entry \n");

	int neighbor_index;
	int k_prime_index;
	int64_t k_prime;
	int capacity;

	int index = pageScan(tableid, n);
	pagenum_t p= b_page.parent;
	int parent = pageScan(tableid, p);
	int head = pageScan(tableid, 0);

	//그 페이지에서 키를 지운다 
//printf("\nremove entry from node call \n");
	remove_entry_from_node(tableid,n, key);

	//지운노드 루트노드였을 때 
	if (n == b_head.root_page) {
//printf("\nadjust root call \n");
		setDirty(index);
		clearPin(index);
		clearPin(head);
		clearPin(parent);
		return adjust_root(tableid,n);
	}

	/*
	 삭제 후 보존할 노드의 최소 허용 크기를 결정한다.근데 우리는 delay merge를 할거니까 필요x
	 키 개수가 0 이상이라면 그냥 merge할필요 x
	 */
//printf("delete entry : page numkey = %d\n",b_page.num_key);
	if (b_page.num_key > 0) {
//printf("\ndelete entry  : numkey>0 \n");
		setDirty(index);
		clearPin(index);
		clearPin(head);
		clearPin(parent);
//printf("\ndelete entry  : return to caller \n");
		return index;
	}

	//key개수가 0이 된경우

	//이웃을 받아온다
	neighbor_index = get_neighbor_index(tableid,n);

	//key프라임 결정--- 아마 들어가야
	//-1이라는건 왼쪽이웃이 없어서 오른쪽 이웃을 택하는경우
	//여기서 키프라임인덱스는 사라질 키의 부모쪽 인덱스
	//맨왼쪽의 경우는 -1번째 키가 없기때문에 0번째 키를 삭제시켜서 
	//해당 인덱스의 child가 right_left로 들어갈 수 있게 해준다.
	k_prime_index = neighbor_index == -2 ? 0 : neighbor_index + 1;
	//키프라임은 위의 받아온 인덱스를 갖고 부모쪽에 저장된 내 키갖고 오기
	//대신 내가 맨오른쪽이라면 자신의 키가 없으므로 가장처음 키인 0번째키로 설정
	k_prime = b_parent.record[k_prime_index].key;

	//-2이라면 자신이 제일 왼쪽 이웃이 없으므로 오른쪽에 옮긴다
	int real_nei=neighbor_index;
	if (neighbor_index == -2)real_nei = 0;

	pagenum_t nei_p = b_parent.branch[real_nei].child;
	int nei = pageScan(tableid, nei_p);

	capacity = b_page.is_leaf ? leaf_order : node_order;

	//찐이웃인덱스 안넘겨줌 -2로 판별가능하게 넘겨줌
	/* Coalescence. */
	//이경우는 합쳤는데 개수 ok일때
	if (b_nei.num_key + b_page.num_key < capacity) {
//printf("\ndelete entry : coal call \n");
		setDirty(index);
		clearPin(index);
		clearPin(head);
		clearPin(parent);
		clearPin(nei);
		return coalesce_nodes(tableid,n, nei_p, neighbor_index, k_prime);
	}

	/* Redistribution. */
	//합쳣는데 개수 초과로 재조정
	else
	{
//printf("\ndelete entry : redistribute call \n");
		setDirty(index);
		clearPin(index);
		clearPin(head);
		clearPin(parent);
		clearPin(nei);
		return redistribute_nodes(tableid,n, nei_p, neighbor_index, k_prime_index, k_prime);
	}
}


