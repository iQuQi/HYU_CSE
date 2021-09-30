	


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
#include <errno.h>
#include "file.h"
#include "bpt.h"
//�Լ� ������ 0���� ���н� -1 ����

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

	//�� ���̺��� ���������� ���� ��ũ�� �����ش�.
	for (int i = 0; i < b_M.frame_capacity; i++)
	{
//printf("closetable2 -- cmp > framArray[i].table id / close id = %d/%d\n",b_M.frameArray[i].table_id , table_id);
		
		if (b_M.frameArray[i].table_id == table_id) {
		b_M.frameArray[i].ispinned=0;
 		pageDrop(i);
		}
	}

	//���̺� �迭���� �����ش�
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
	//���̺� �ö���ִ� ������ŭ close table
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
	//�����Ҵ� �����ش�, calloc�� ��� 0�� �ʱ�ȭ... �׷��� Ȥ�� �𸣴Ϥ�
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

	//����� �о�´�.
	int root;
	int head;
	//O_creat ��忡���� ����° ���ڿ� ���Ͽ� ���ٱ��� ��������
	//������ ���� ��� ���� �������ִµ� ������ �����Ѵٸ� ������ ����(O_EXCL)

	
	//���� ���ϸ��� �����ϴ��� üũ
	int i;
	for (i = 0; i < 10; i++)
	{
		if (!strcmp(pathname, b_M.table[i].path)) {
			break;
		}
	}
	
//printf("open table : i= %d\n",i);
	//������ �������� ���� ���̺�
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
		//���������� ���ȴٸ�
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
			//��� ������ ���� ī��Ʈ
			b_head.page_num = 1;
			b_head.free_page = 0;
			b_head.root_page = 0;

			//��� dirty/pin set
			clearPin(head);
			setDirty(head);

			return table_id;
		}

		//�����Ǿ��ִٸ� �ٽ� �о�´�.
		b_M.table[i].fd = open(pathname, O_RDWR);
		table_fd = b_M.table[i].fd;

		//�������� �о������
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
			//��Ʈ�������� ���ۿ� �÷��ش�
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

	//������ �������� �־� ������ ����Ǿ��ִ� ��� fd�� �����ؼ� ����
	else {
//printf("open table : already open-1\n");
		b_M.table[i].fd = open(pathname, O_RDWR);
//printf("new fd : %d\n",b_M.table[i].fd);
		int table_fd = b_M.table[i].fd;
		int table_id = b_M.table[i].id;

		if (table_fd > 0) {
//printf("open table : already open-2\n");
			//����� ���ۿ� �÷��ش�
			head = pageScan(table_id, 0);
			b_M.table[i].isopen = 1;
			b_M.table_use++;

			//��Ʈ�������� ���ۿ� �÷��ش�
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
	//��� ��������� Ȯ��
	if (pinCheck() == FAIL) {
		return FAIL;
	}

	//��ũ�� ����Ʈ���� ����
	int pre = b_index.pre;
	int next = b_index.next;

	//drop�� �������� ��忴�ٸ�
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
	//���� �̾��ٸ�
	else if (next == TAIL) {
//printf("pagedrop tail \n");
		b_M.LRU_tail = b_index.pre;
		b_M.frameArray[b_index.pre].next = TAIL;
	}
	//�Ѵ� �ƴѰ��
	else{
//printf("pagedrop normal \n");
		b_M.frameArray[pre].next = next;
		b_M.frameArray[next].pre = pre;
	}
	
	//�ʱ�ȭ
	b_index.pre = -1;
	b_index.next = -1;

	//��Ƽ�ΰ�� ��ũ�� ���ְ� �����ش�
	if (b_index.isdirty) {
//printf("pagedrop dirty \n");
//printf("parent : %ld\n",b_page.parent);
		file_write_page(b_M.table[b_index.table_id-1].fd, b_index.page_num, &b_page);
	}
	b_index.table_id = 0;
	b_index.page_num = 0;

	//�ʱ�ȭ
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
	//���ϴ� �������� �������� �ö��ִ��� Ȯ��
	//������ pick
	//������ ��ũ���� �ҷ��� �÷������
	//�ҷ����� ���۸Ŵ��� ����

	for (int i = 0; i < b_M.frame_capacity; i++)
	{
		if (b_M.frameArray[i].table_id == table && b_M.frameArray[i].page_num == pagenum){
//printf("scan_already exist : i=%d\n",i);
			setPin(i);
			return i;
		}
	}


	//���ڸ��� ������� ���ۿ� �÷��ش�.
	if (b_M.frame_capacity > b_M.use_num) {

		for (int i = 0; i < b_M.frame_capacity; i++)
		{
			//��������� ���� �������� ã���� �ű�� �������� �����´�
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
	
	//���ڸ��� ���°��
	else {
//printf("scan_space full\n");
		//��� ������ΰ��
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
		
		//�뷮�̲������ drop
		pageDrop(victim);
//printf("scanend\n");


	}
	

//printf("final victim : %d, pagenum : %ld\n",victim,pagenum);
	//��� �����ӿ� �� �������� �о���� ���� ����
	setPage(table, pagenum, victim);//read�ؼ� tableid, pagenum ����
	setDirty(victim);
	setPin(victim);

	pageLoad(victim);
//printf("pagescan end\n");
	return victim;
}

//�� �������� ���ۿ� �Ǿ��� ������ next/pre���� ����
int pageLoad(int index)
{
//printf("\npage load : index=%d \n",index);
	b_M.use_num++;

	if (b_M.use_num == 1) {
//printf("page load only use1_return\n");
		b_M.LRU_head = index;
		b_M.LRU_tail = index;
		b_index.pre = HEAD; // �ڽ��� �Ǿ�
		b_index.next = TAIL; //�ڽ��� �ǳ�

		return SUCCESS;
	}
//printf("page load more than one..\n");
	//���� ���� ������
	b_index.pre = HEAD; // �ڽ��� �Ǿ��̶�� �Ҹ�
	b_index.next = b_M.LRU_head;

	//�յڷ� ����
	b_M.frameArray[b_M.LRU_head].pre = index;
	b_M.LRU_head = index;
	

//bufInfo();
//printf("pageload end\n");
	return SUCCESS;
}


//��� �������� ��������� üũ�ϴ� �Լ�
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
	//����
	return FAIL;
	
}


//�������� �о�ͼ� ������Ʈ
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

//�Ʒ��� ���� set/clear
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
	//����� �޾ƿ´�.
	if(b_M.table[tableid-1].isopen==0) {
		printf("File Is Closed\n");
		return FAIL;
	}
//printf("\ndb_insert - 1\n");
	int head = pageScan(tableid, 0);
//printf("db_insert - head find success\n");
	//ã�� ��� -- �̹� �ִ� ���

//find case--- duplicate
	if (!db_find(tableid,key, value)) {
//printf("db_insert - duplicate\n");
		clearPin(head);
		return FAIL;
	}

	//��ã����� -- ���� �� ����
	//Ʈ���� �ȸ������ �ִ°��(ù ������ ���)
	if (b_head.root_page == 0) {
//printf("db_insert - start new tree\n");
		clearPin(head);
		return start_new_tree(tableid,key, value);
	}

	//Ʈ���� ��������ִ� ���
	//Ű�� �� ���������� �о���δ�
	int leaf = find_page(tableid,key);
	pagenum_t l = b_M.frameArray[leaf].page_num;
//printf("db_insert - leaf find success\n");
	//printf("db insert here -- leaf num = %ld\n",leaf);

	//���� �������� ���� �ڸ��� �ִ� ���
	if (b_leaf.num_key < leaf_order) {
//printf("db_insert - insert into leaf\n");
//printf("num key = %d and leaf order = %d\n",b_leaf.num_key,leaf_order);
		insert_into_leaf(tableid,l, key, value);
		clearPin(head);
		clearPin(leaf);
		return SUCCESS;
	}

	//���� �ڸ��� ���ٸ� �ɰ�����
//printf("db_insert - insert into after splitting\n");
	insert_into_leaf_after_splitting(tableid,l, key, value);
	clearPin(head);
	clearPin(leaf);
	return SUCCESS;

	//�� Ű�� �̹� �ִ��� Ȯ���Ѵ�.
	//���ٸ� �� ���ڵ带 �����Ѵ�
	//���� ��Ʈ��嵵 ���ٸ� �� Ű�� ��Ʈ�� ���� ������ش�
	//�� Ű�� �� �������� �����´�
	//insert into leaf����
	//�������� leaf after splitȣ��

}

int db_find(int tableid,int64_t key, char * ret_val) {
	int i;
	if(b_M.table[tableid-1].isopen==0) {
		printf("File Is Closed\n");
		return FAIL;
	}
//printf("\ndb_find-1\n");
	//�� Ű�� ����������� �ִ� ������������ �����´�
	int find_p = find_page(tableid,key);
//printf("db_find --- find page success\n");
	//��Ʈ���� ���
	if (find_p == FAIL) {
//printf("db_find- empty\n");
		clearPin(find_p);
		return FAIL;
	}

	//Ű�� �߰��ϸ� �����
	for (i = 0; i < b_M.frameArray[find_p].frame_p.num_key; i++)
	{
		if (b_M.frameArray[find_p].frame_p.record[i].key == key) break;
	}
//printf("db_find-2\n");
	//�ε����� ������ �ƴ϶�� ã�����Ѱ� - break�� �������� �ƴ϶�� ��
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
	//���� �޾ƿ� �������� null�̶�� ��������
	//�ִٸ� �� ������ ������ key�� ��ġ�ϴ� ���� ã�´�
	//for ������ Ű�� �ε����� ã�´� i 5 
	//�� �ε����� numkey�� ���ٸ� ��ã���� --- -1����
	//�ƴ϶�� val�� �����ϰ�  0�̷�
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

	//��Ʈ��
	if (leaf==FAIL) {
//printf("empty\n");
		clearPin(leaf);
		return FAIL;
	}

	int ch = db_find(tableid,key, tmp);
	//ã�Ҵٸ�
	if (ch == 0) {
//printf("delete entry call\n");
		delete_entry(tableid,l, key);
		free(tmp);
		return SUCCESS;
	}

	//��ã�Ҵٸ� ���������
	else {
//printf("not find\n");
		free(tmp);
		return FAIL;
	}

}

///////////////
//���߿� ����κ�
queue * q;
void print_tree(int table_id) {
	printf("\n<print tree>\n");
	pagenum_t nnum, rnum;

	//����޾ƿ���
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
	//�� ������������ �޾ƿ´�
	int root = make_internal_page(tableid);
	int head = pageScan(tableid, 0);
	int right = pageScan(tableid, r);
	int left = pageScan(tableid, l);



	b_head.root_page = b_M.frameArray[root].page_num;
//printf("\ninsert into new root 2 -- change rootpage=%ld\n",b_head.root_page);
	b_root.parent = 0;//��Ʈ flag
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


//ó�� ����
int start_new_tree(int tableid,int64_t key,char * val) {

//printf("\nstart new tree 1 \n");

	//�� ������������ �޾ƿ´�
	int leaf = make_leaf_page(tableid);
//printf("leaf : %d\n",leaf);
	int head = pageScan(tableid, 0);
//printf("start new tree 2 \n");
	//��� ����
	b_head.root_page =b_M.frameArray[leaf].page_num;

//printf("start new tree - root : %ld \n",b_head.root_page);
	//b_head.page_num++;

	//���������� ����
	b_leaf.parent = 0;
	b_leaf.num_key = 1;
	b_leaf.record[0].key = key;

	//char *strcpy(char *dest, const char *src);
	//scr�� dest�� ����
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
	//����� �޾ƿ´�
	int head = pageScan(tableid, 0);

	//��Ʈ�� �ϰ��
	if (b_head.root_page == 0) {
//printf("find page - empty\n");
		clearPin(head);
		return FAIL;
	}

	//��Ʈ��������
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
	//while �� �����ٴ°� ���� ���� ����
	//�̶� ��ȯ�Ǵ� ���� ���������� ���� Ű�� ������� ���� �ְ� ���� ���� �ִ�
	//���� ��� 9�� ã�� ���, 7�̻� 10�̸��� Ű���� ����־���ϴ� �������� ��ȯ�Ǿ
	//�ű⿡ 9�� �ִٴ� ������ x

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

//���⼭�� ������� ����x ---alloc�� �ϸ鼭 ���������� �����Ϸ�/�Ѱ����� ����x
int make_internal_page(int table_id) {

//printf("\nmake internal page1\n");
	//�������� ������ ����
	//����/���ͳ� - numkey, {key, },parent, right_left, isleaf
	//��� - ��Ʈ,����,�Ѱ���
	int newpage = file_alloc_page(table_id);
//printf("new alloc internal pagenum : %ld\n",b_M.frameArray[newpage].page_num);
	
	//���ʱ�ȭ 
	b_M.frameArray[newpage].frame_p.is_leaf = 0;
	b_M.frameArray[newpage].frame_p.num_key = 0;

	//���� �θ� ���� �ȳ�
	b_M.frameArray[newpage].frame_p.parent = 0;

	//�ڱ���� ���� ���� �������� ����x
	b_M.frameArray[newpage].frame_p.right_left = 0;

	//Ű+offset���� ���� �������ʿ�x
	setDirty(newpage);
	clearPin(newpage);
	return newpage;
}


int make_leaf_page(int tableid) {
//printf("\nmake leaf page1\n");
	//�� ������ ���� ��
	int leaf = make_internal_page(tableid);
//printf("new alloc leaf pagenum : %ld\n",b_M.frameArray[leaf].page_num);

	//������ set
	b_M.frameArray[leaf].frame_p.is_leaf = 1;
	setDirty(leaf);
	clearPin(leaf);
	return leaf;
}
/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 ������ Ű ���ʿ� �ִ� ��忡 ���� �θ� �������� �ε����� ã�� ���� insert_into_parent�� ���Ǵ� ����� ���.
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
	
	//������ġ�� ã�´�.
	while (insertion_point < b_leaf.num_key &&  b_leaf.record[insertion_point].key < key)
		insertion_point++;

//printf("insert into leaf : lnsert point = %d\n",insertion_point);
	//������ġ �ڸ��� �о ����ش�
	for (int i = b_leaf.num_key; i > insertion_point; i--) {
		b_leaf.record[i].key = b_leaf.record[i - 1].key;
		strcpy(b_leaf.record[i].val, b_leaf.record[i - 1].val);
		
	}

	//�� ����� ��ġ�� ����
	b_leaf.record[insertion_point].key = key;
	strcpy(b_leaf.record[insertion_point].val, val);
//printf("insert into leaf---before numkey++ : %d\n",b_leaf.num_key);
	b_leaf.num_key++;
//printf("insert into leaf---after numkey++ : %d\n",b_leaf.num_key);

	//������ leaf �ٽ� write�ϰ� free
	setDirty(leaf);
	clearPin(leaf);
	
	return SUCCESS;
}


int insert_into_leaf_after_splitting(int tableid,pagenum_t l, int64_t key, char * val) {
//printf("\ninsert into leaf after splitting 1\n");
	//���� �ִ� ���� ������ֱ�
	int leaf = pageScan(tableid, l);
	int insertion_index, split, new_key, i, j;

	//�ӽð��� ����
	key_val* tmp_record = (key_val *)malloc(33*sizeof(struct record));

	//�ɰ��� �־��� �� ���� ������ֱ�
	int new_p = make_leaf_page(tableid);
	pagenum_t n_p = b_M.frameArray[new_p].page_num;
//printf("insert into leaf after splitting 2 --- leaf pagenum = %ld\n",n_p);
	//tmpŰ�����Ϳ� �����͸� ����Ű�� �����͸� �����д� ? �� ---
	//�״ϱ� ����3���� ���� �ִµ� �ϳ��� �����°�� 4��¥���� �����Ҽ��ִ� temp�� �����
	//�װ� split�ؼ� �ΰ��� �ɰ��� �־��شٴ� ����

	//������ġ ã��
	insertion_index = 0;
	while (insertion_index < leaf_order && b_leaf.record[insertion_index].key < key)
		insertion_index++;

//printf("insert into leaf after splitting 3 -- insert index = %d\n",insertion_index);
	//������ġ�ΰ�� �ϳ������ leat�� ���� �����ؼ� �����ͼ� ���� 
	for (i = 0, j = 0; i < b_leaf.num_key; i++, j++) {
		if (j == insertion_index) j++;
		tmp_record[j].key = b_leaf.record[i].key;
		strcpy(tmp_record[j].val, b_leaf.record[i].val);
	}
	//�� ������ġ�� �־��ֱ�
	tmp_record[insertion_index].key = key;
	strcpy(tmp_record[insertion_index].val, val);


	//���� ������ ���� �ٽ� ä���ش�
	b_leaf. num_key= 0;

	//�������� ���ϱ�
	split = cut(leaf_order);
//printf("insert into leaf after splitting 4 -- split=%d\n",split);
	//���� ������ �ɰ��κ��� ������ �־��ֱ�
	for (i = 0; i < split; i++) {
		strcpy(b_leaf.record[i].val , tmp_record[i].val);
		b_leaf.record[i].key = tmp_record[i].key;
		b_leaf.num_key++;
	}
	//�� ������ �ɰ��κ��� ������ �κ��� �־��ֱ�
	for (i = split, j = 0; i <= leaf_order; i++, j++) {
		strcpy(b_M.frameArray[new_p].frame_p.record[j].val , tmp_record[i].val);
		b_new.record[j].key = tmp_record[i].key;
		b_new.num_key++;
	}

	//tmp���� free
	

	//�� ������ ���������� ��������ش�  ���� ������ ���������� ����Ű�� �ִ�
	//���� �κ��� ���θ���� ����ų�� �ֵ��� ������ ���� �ص־���
	b_new.right_left = b_leaf.right_left;
	b_leaf.right_left = n_p;

	//���� ������ �籸�������Ƿ� �������� ����ִ� �κе��� ������⶧����
	//�ű⿡ ���� ������ ���ش�
	for (i = b_leaf.num_key; i < leaf_order ; i++) {
		b_leaf.record[i].key = -1;
		
	}


	//���� ������ ���ʺκ� ����
	for (i = b_new.num_key; i < leaf_order ; i++) {
		b_new.record[i].key = -1;
		
	}

	//���� ������� ������ �θ�� ��������ְ� newŰ ���� 
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
	//n_p�� parent�� �޾ƿ�
	int page_parent = pageScan(tableid, n_p);

	//left �ε������� ��ĭ�� ���������� �̷Ｍ �ڸ��� Ȯ���Ѵ�
	for (i = b_page_parent.num_key; i > left_index; i--) {
		b_page_parent.branch[i + 1].child = b_page_parent.branch[i].child;
		b_page_parent.branch[i + 1].key = b_page_parent.branch[i].key;
	}

	//Ű�� �����͸� ����
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
	//into parent�Լ����� old�� parent�� �Ѱ���
	int i, j, split;
	int64_t k_prime;
	
	//���� �ִ� ��� ������ֱ�
	int old_p = pageScan(tableid, old);

	//�ɰ��� �־��� �� ��� ������ֱ�
	int new_p = make_internal_page(tableid);
	pagenum_t n_p = b_M.frameArray[new_p].page_num;
// printf("\ninsert into node after splitting 2 -- alloc complete : %d \n",new_p);
	/*
	 ���� �� Ű�� �����͸� ������ ��� ���� �ùٸ� ��ġ��
	 ������ �� �ִ� �ӽ� Ű�� �����͸� ����ʽÿ�.
	 �׷� ���� �� ��带 �����ϰ� Ű�� �������� ������ ���� ��忡,
	 ������ ������ �� ��忡 �����Ͻʽÿ�.
	 */
	 //�ӽð��� ����
	key_child * tmp_key_child = (key_child *)malloc(250 * sizeof(struct key_child));

	//������ ��ġ�� ����ش�
	for (i = 0, j = 0; i < b_old.num_key + 1; i++, j++) {
		if (j == left_index + 1) j++;
		tmp_key_child[j].child = b_old.branch[i].child;
	}
	for (i = 0, j = 0; i < b_old.num_key; i++, j++) {
		if (j == left_index+1) j++;
		tmp_key_child[j].key = b_old.branch[i].key;
	}

	//������ ��ġ�� ����
	tmp_key_child[left_index + 1].child = right;
	tmp_key_child[left_index+1].key = key;

	/* Create the new node and copy
	 * half the keys and pointers to the
	 * old and half to the new.
	 �� ��带 �����ϰ� Ű�� �������� ������ ���� ��忡, ������ ������ �� ��忡 �����Ͻʽÿ�.
	 */
	 //�ɰ��� ��ġ ã��
	split = cut(node_order);
	//���� ��� ����ֱ�
	b_old.num_key = 0;

	//�����ִ� ��� ä���
	for (i = 0; i < split-1; i++) {
		b_old.num_key++;
		b_old.branch[i].key = tmp_key_child[i].key;
		b_old.branch[i].child= tmp_key_child[i].child;
		
	}

	//������ ���� �ڽ��� �ڽ��� ���� ������ �׳� �ø��� �۾��� ������ ����x
	//�׷��� ���ͳ��� ��� �ڽ��� ���� �ö󰡰� �ǹǷ�
	//�ڽ��� �ڽ����� ������ �ʿ�


	k_prime = tmp_key_child[split-1].key;//�ö� ģ���� k_primed�� ����

	
	//���θ��� ���ä���
	b_new.num_key = 0;
	//split ���� ���� ����
	for (++i, j = 0; i <= node_order; i++, j++) {
		b_new.num_key++;
		b_new.branch[j].key = tmp_key_child[i].key;
		b_new.branch[j].child = tmp_key_child[i].child;
		
	}
	
	//�ø� Ű�� ���� ����Ű�� �ִ� �ڽ��� ���θ��� ����� right_left(rightmost)�� ����Ű�� ����
	b_new.right_left= tmp_key_child[split-1].child;

	//���θ��� ��� �θ������ֱ�
	b_new.parent = b_old.parent;

	//���θ��� ����� �ڽ��� �θ����� ������
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

	return insert_into_parent(tableid,old, k_prime, n_p);//�ö� ģ���� �����ؼ� �θ� ����
}


int insert_into_parent(int tableid,pagenum_t l, int64_t key, pagenum_t right) {
//printf("\ninsert into parent 1\n");
	int left_index;
	pagenum_t p_page;

	int left = pageScan(tableid, l);
	p_page = b_left.parent;

//printf("insert into parent 2 : left=%d,parent page=%ld\n",left,p_page);

	//����Ʈ�� �־���ϴ� ���(�� left�� ��Ʈ(internal)���� ���)
	if (p_page == 0) {
//printf("insert into new root call\n");
		clearPin(left);
		return insert_into_new_root(tableid,l, key, right);
	}

	//�ƴѰ�� �θ� �޾ƿ´�
	int parent = pageScan(tableid, p_page);
	//left�� �θ��ʿ��� ���° �ε����� �ִ��� Ȯ��
//printf("insert into parent : parent = %ld\n",p_page);
	left_index = get_left_index(tableid,p_page, l);

	//�θ��������� �÷��ش�
	if (b_parent.num_key < node_order) {
//printf("insert into node call\n");
		clearPin(left);
		clearPin(parent);
		return insert_into_node(tableid,p_page, left_index, key, right);
	}
	 //�θ�� �ϳ� �÷ȴµ� �ű⼭�� ������� �ٽ� �ɰ������
	clearPin(left);
	clearPin(parent);
//printf("insert into node splitting call\n");
	return insert_into_node_after_splitting(tableid,p_page, left_index, key, right);
}





///////////////////////////////////////////////////////////////////////
// DELETION.////////////////////////////////////////////////////////
/*
������ ���� ��ƿ��Ƽ ���. ����� ���� ����� �̿�(�ú�) �ε����� �ִ� ���
 ������ �˻��Ͻʽÿ�.
 �׷��� ���� ���(���� ���� ������ ���� ���)�̴�. -1�� ��ȯ�Ͽ� �� Ư�� ��ʸ� ��Ÿ����.
 */

int get_neighbor_index(int tableid,pagenum_t n_p) {

	int new_p = pageScan(tableid, n_p);
	pagenum_t p = b_new.parent;
	int parent = pageScan(tableid, p);

	int i;

	/*
	Ű�� �ε����� n_p�� ����Ű�� �θ� �ִ� �������� �����ε������� ��ȯ.
	 n�� ���� ������ ������ ���, �̴� -2�� ���ƿ´ٴ� ���� �ǹ��Ѵ�.
	 
	*/

	if (b_parent.right_left == n_p) {
		clearPin(new_p);
		clearPin(parent);
		return -2;
	}

	for (i = 0; i < b_parent.num_key; i++) {
		if (b_parent.branch[i].child == n_p) {
			//�̶� i�� �θ𿡰Լ� �޾ƿ� �ڱ��ε���
			clearPin(new_p);
			clearPin(parent);
			return i - 1;//���̰� �θ��ʿ��� Ȯ���� key�� �ٷ� �������� �ε���
		}
	}

}

int remove_entry_from_node(int tableid, pagenum_t n, int64_t key) {
	int i;

//printf("\nremove entry from node start\n");
	int index = pageScan(tableid,n);

	// Remove the key and shift other keys accordingly.
	i = 0;
	//Ű��ġ ã�� Ű�� ��ܿ��鼭 ������ش�
	//internal�ΰ��
	if(!b_page.is_leaf) {
//printf("\nremove entry from node : not leaf\n");
		while (b_page.branch[i].key != key)
			i++;
		//Ű�� child  ����ֱ�
		for (++i; i < b_page.num_key; i++) {

			b_page.branch[i - 1].child= b_page.branch[i].child;
			b_page.branch[i - 1].key = b_page.branch[i].key;

		}
	}
	//�����ΰ��
	else if (b_page.is_leaf) {
//printf("\nremove entry from node : leaf\n");
		while (b_page.record[i].key != key)
			i++;

		//Ű�� val ����ֱ�
		for (++i; i < b_page.num_key; i++) {

			strcpy(b_page.record[i - 1].val, b_page.record[i].val);
			b_page.record[i - 1].key = b_page.record[i].key;

		}
	}



	// Remove the pointer and shift other pointers accordingly.
	// First determine number of pointers.

	// One key fewer.
	b_page.num_key--;

	// Set the other pointers to NULL for tidiness.//������ ����

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
	//����� ��Ʈ��忡 �������� ���������� - �̹� ������ ��������
	int root = pageScan(tableid, r);
	int new_root = pageScan(tableid, b_root.right_left);
	int head = pageScan(tableid, 0);

	/* Case: nonempty root.
	Ű�� �����Ͱ� �̹� �������ְ� �Ұ� ���� ���
	 * Key and pointer have already been deleted,
	 * so nothing to be done.
	 */
	 //������� ���� ��Ʈ ����-- �״ϱ� ������ ������ �ߴµ� ������ ��Ʈ�� ������� ������ ������
	 //�����Ƿ� �׳� ���̶�� �Ҹ�
	if (b_root.num_key > 0) {
//printf("adjust root : not problem!\n");
		clearPin(head);
		clearPin(root);
		clearPin(new_root);
		return r;
	}

	/* Case: empty root. �״ϱ� ������ ������ �Ѱ���� ���ϱ� ��Ʈ�� ����������
	 �״ϱ� �Ʒ��ڽ��� ������ ��Ʈ�� �÷������
	 �ڽĵ� �������� ��°�� ����ִ� Ʈ���� �Ȱ���
	 */
	 //����ִ� ��Ʈ�ΰ��
	 // If it has a child, promote 
	 // the first (only) child
	 // as the new root.

	 //�ڳడ �ִ� ��� ù ��°(only) �ڽ��� �� ��Ʈ�� �����
	 //��Ʈ�� ������ �ƴҶ�
	if (!b_root.is_leaf) {
//printf("adjust root : not leaf\n");
		file_free_page(tableid,r);
		b_head.root_page = b_root.right_left;
		b_M.frameArray[new_root].frame_p.parent = 0;
		
	}
	//
	// If it is a leaf (has no children),
	// then the whole tree is empty.

	//��Ʈ�� �����϶� --- child�� ������ ���̰� ��� Ʈ���� ����ִٴ� ��
	//��Ʈ�������� ������
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
	//���� �����ȷ�Ʈ
	return new_root;
}

int coalesce_nodes(int tableid,pagenum_t n, pagenum_t n_p, int neighbor_index, int64_t k_prime) {
	//�޾ƿ� kprime�� ����������� �θ𿡼��� Ű
	//�޾ƿ� �̿� �ε����� ���ε����ƴ� -2 ����
	//n_p�� �� �̿�
//printf("\ncoal node\n");
	int i, j, neighbor_insertion_index, n_end, new_p, nei, t_p;
	pagenum_t tmp;

	pagenum_t change_neighbor;
	pagenum_t change_die;

	 //�����尡 ���� �����̸� �����ʳ��� ��ģ��
	//��� ������ ��忡 ���̴°� �ƴϰ� ������ �������� ���δ�.
	//�׸��� ������ ��带 �����ϴ� ����
	if (neighbor_index == -2) {

		//nei�ȿ��� ������ 
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

	//�θ��������ѹ��� �޾ƿ´�.
	pagenum_t p = b_new.parent;

	/* 
	 n���� Ű �� �����͸� �����ϱ� ���� ���� ����� ������.
	 n�� �̿��� n�� ���� ������ �ڽ��� ��쿡 �ڸ��� �ٲپ��ٴ� ���� ���.
	 */

	neighbor_insertion_index = b_nei.num_key;

	 //������ �ƴѰ��
	if (!b_new.is_leaf) {
//printf("coal node : not leaf\n");
		b_nei.branch[neighbor_insertion_index].key = k_prime;
		b_nei.branch[neighbor_insertion_index].child = b_new.right_left;
		b_nei.num_key++;
		
		//�̿����� �Ű��ִ� ����
		for (i = neighbor_insertion_index + 1, j = 0; j < b_new.num_key; i++, j++) {
			b_nei.branch[i].key = b_new.branch[j].key;
			b_nei.branch[i].child = b_new.branch[j].child;
			b_nei.num_key++;
			
		}

		//�������� �� �Űܿ����Ƿ� �θ� ��� �����Ұ���-- �θ������ x

		 //���� n�� �ڽĵ鵵  neighbor�� ����Ű�� ��������
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

	//�����ΰ��
	/*
	 key�� 0�� �̹Ƿ� �ٷ�free ,�׸��� �ùٸ� right page�� ����Ű�� �缳��
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
			//�̿����� �Ű��ִ� ����
			for (i = neighbor_insertion_index, j = 0; j < b_new.num_key; i++, j++) {
	
				b_nei.record[i].key = b_new.record[j].key;
				strcpy(b_nei.record[i].val, b_new.record[j].val);
				b_nei.num_key++;
			
			}
			b_nei.right_left= b_new.right_left;
			file_free_page(tableid,change_die);
		
		}
		

		
	}

	//n�� ������ ���������ش�
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

	//delay merge������  node�� ���� leaf�� �� x
	pagenum_t tmp;
	int i, t_p;
	int index = pageScan(tableid, n);
	int nei = pageScan(tableid, n_p);
	pagenum_t p = b_page.parent;
	int parent = pageScan(tableid, p);

	/* Case: n has a neighbor to the left.
		�̿��� ������ Ű-���� ���� �̿��� ������ ������ n�� ���� ������ ���ʽÿ�.
	�״ϱ� �̰� '�̿� - ��' �̷��� ������ �̿��� �ϳ��� ������ ��ܿ��°���
	�� ��ġ�� ��ġ�ϱ� ��ġ�� ��� �������� ��
	 */

	if (neighbor_index != -2) {
//printf("\nredistribute node\n");
		//������ �ƴѰ�� 
		//����̶� ������ �ƴ϶�°� �ڽĵ� �ִٴ� �Ҹ��ϱ� �ڽĵ鵵 �Ű��ִ� �۾��ʿ� - tmp
		if (!b_page.is_leaf) {
			//�ϴ� �տ� ���ڸ� ����ߵǴϱ� ��ĭ�� �߹о��ش�
//printf("\nredistribute node\n");

			//child�����͵��� �̵�
			b_page.branch[0].child = b_page.right_left;
			b_page.right_left = b_nei.branch[b_nei.num_key - 1].child;
			
			//������ �ڽ��� �θ����� �ٽ�
			tmp = b_page.right_left;
			t_p = pageScan(tableid, tmp);
			b_M.frameArray[t_p].frame_p.parent = n;
			setDirty(t_p);

			//�̿��� Ű�ϳ��� keyprime index�� �θ�� �ø���
			//���ڸ����� �θ��� key prime�� �����´�(�����е� �׸� ����)
			b_page.branch[0].key = k_prime;
			b_parent.branch[k_prime_index].key = b_nei.branch[b_nei.num_key - 1].key;

			//�̿����� �ڿ� ��ĭ�� �ʱ�ȭ
			b_nei.branch[b_nei.num_key - 1].child = -1;
			b_nei.branch[b_nei.num_key - 1].key = -1;

			
		}

		//�����ΰ�� -  ����� �ڸ��� �Ű��ش�
		//�ڽ��� �����Ƿ� �Ű澵�ʿ� ����
		else {

//printf("\nredistribute node\n");
			//������� ������ �ּ�ũ�⸦ �������� ������ �̿����� ��������
			//���������� capacity�� �ʰ��ϰԵǼ� ���Լ��� �Ҹ��µ� 
			//�̹��� ��� ������ ������ numkey�� 0�̹Ƿ� �ʰ��� �Ұ���
			//�� �� �Լ� �ȿ����� leaf�� ���� ����� ���� �ʾƵ� ��
		}
	}

	/* Case: n is the leftmost child.
	�״ϱ�  "�� - �̿�" �̷��� ������ �̿��� ������ʾտ�Ҹ� ���� ���ϵ������� ��ܿ��°���
	�����´ٰ� ���� -> �̰�� ���� �ڿ� �ִ°� �̹Ƿ� ���� ��ĭ�� �������ʿ�x ���� ��캸�� ��������
	 */
	 //�Ʒ��� key�������� �̿��� ��������  �θ������� Ű
	else {
//printf("\nredistribute node\n");
		//�����ΰ�� - �ؿ� �ڽĵ鿡 ���� ���� �ʿ�x
		if (b_page.is_leaf) {

//printf("\nredistribute node\n");
			//������� ������ �ּ�ũ�⸦ �������� ������ �̿����� ��������
			//���������� capacity�� �ʰ��ϰԵǼ� ���Լ��� �Ҹ��µ� 
			//�̹��� ��� ������ ������ numkey�� 0�̹Ƿ� �ʰ��� �Ұ���
			//�� �� �Լ� �ȿ����� leaf�� ���� ����� ���� �ʾƵ� ��
		}

		//������ �ƴѰ�� �ؿ��ڽĵ鿡 ���� ������ �ʿ�
		else {
//printf("\nredistribute node\n");
			//�θ��ʿ��� keyprime�� ������� �̿��� child pointer�� �����´�
			b_page.branch[0].key = k_prime;
			b_page.branch[0].child =b_nei.right_left;
			b_nei.right_left = b_nei.branch[0].child;
		
			tmp = b_page.branch[0].child;
			t_p = pageScan(tableid, tmp);
			b_M.frameArray[t_p].frame_p.parent = n;
			b_parent.branch[k_prime_index].key = b_nei.branch[0].key;
			setDirty(t_p);


			//�̿��� ���� ���� ��ĭ�� ����������Ƿ� ��ܿ;���
			for (i = 0; i < b_nei.num_key-1; i++) {
				b_nei.branch[i].key = b_nei.branch[i + 1].key;
				b_nei.branch[i].child = b_nei.branch[i + 1].child;
			}
			b_nei.branch[i].key=-1;
			b_nei.branch[i].child=-1;
		}
	}


	/*
	 n�� ���� �� ���� ����� �� ���� �����͸� �� ������ ������,
	 �̿��� ���� �� ���� �� ���� ������ �ִ�.
	 �״ϱ� �ϳ� �����Դ� �̸�
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

	//�޾ƿ� ���������� �� Ű�� ����
//printf("\ndelete entry \n");

	int neighbor_index;
	int k_prime_index;
	int64_t k_prime;
	int capacity;

	int index = pageScan(tableid, n);
	pagenum_t p= b_page.parent;
	int parent = pageScan(tableid, p);
	int head = pageScan(tableid, 0);

	//�� ���������� Ű�� ����� 
//printf("\nremove entry from node call \n");
	remove_entry_from_node(tableid,n, key);

	//������ ��Ʈ��忴�� �� 
	if (n == b_head.root_page) {
//printf("\nadjust root call \n");
		setDirty(index);
		clearPin(index);
		clearPin(head);
		clearPin(parent);
		return adjust_root(tableid,n);
	}

	/*
	 ���� �� ������ ����� �ּ� ��� ũ�⸦ �����Ѵ�.�ٵ� �츮�� delay merge�� �ҰŴϱ� �ʿ�x
	 Ű ������ 0 �̻��̶�� �׳� merge���ʿ� x
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

	//key������ 0�� �Ȱ��

	//�̿��� �޾ƿ´�
	neighbor_index = get_neighbor_index(tableid,n);

	//key������ ����--- �Ƹ� ����
	//-1�̶�°� �����̿��� ��� ������ �̿��� ���ϴ°��
	//���⼭ Ű�������ε����� ����� Ű�� �θ��� �ε���
	//�ǿ����� ���� -1��° Ű�� ���⶧���� 0��° Ű�� �������Ѽ� 
	//�ش� �ε����� child�� right_left�� �� �� �ְ� ���ش�.
	k_prime_index = neighbor_index == -2 ? 0 : neighbor_index + 1;
	//Ű�������� ���� �޾ƿ� �ε����� ���� �θ��ʿ� ����� �� Ű���� ����
	//��� ���� �ǿ������̶�� �ڽ��� Ű�� �����Ƿ� ����ó�� Ű�� 0��°Ű�� ����
	k_prime = b_parent.record[k_prime_index].key;

	//-2�̶�� �ڽ��� ���� ���� �̿��� �����Ƿ� �����ʿ� �ű��
	int real_nei=neighbor_index;
	if (neighbor_index == -2)real_nei = 0;

	pagenum_t nei_p = b_parent.branch[real_nei].child;
	int nei = pageScan(tableid, nei_p);

	capacity = b_page.is_leaf ? leaf_order : node_order;

	//���̿��ε��� �ȳѰ��� -2�� �Ǻ������ϰ� �Ѱ���
	/* Coalescence. */
	//�̰��� ���ƴµ� ���� ok�϶�
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
	//�ի��µ� ���� �ʰ��� ������
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


