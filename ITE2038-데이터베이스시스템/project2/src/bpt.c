


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


int ch;
int op_fd;

void close_table(int fd){
	int ch=close(fd);
	if(ch!=0) printf("close failed\n");

}


int db_insert(int64_t key, char * value) {
	//printf("db insert 1\n");
	header_page * head = (header_page *)calloc(1, PAGESIZE);

	file_read_page(0, (page_t*)head);


	//찾은 경우 -- 이미 있는 경우

//find case--- duplicate
	if (!db_find(key, value)) {
		//printf("db insert find\n");
		return -1;
	}

	//못찾은경우 -- 넣을 수 있음
	//트리가 안만들어져 있는경우(첫 삽입인 경우)
	if (head->root_page == 0) {
		//	printf("db insert root page\n");
		return start_new_tree(key, value);
	}

	//트리가 만들어져있는 경우
	//키가 들어갈 리프페이지 읽어들인다
	page_t * p = (page_t *)calloc(1, PAGESIZE);
	pagenum_t leaf = find_page(key);
	file_read_page(leaf, p);

	//printf("db insert here -- leaf num = %ld\n",leaf);


	//리프 페이지에 남은 자리가 있는 경우
	if (p->num_key < leaf_order) {
		//printf("db insert-insert into leaf call\n");
//printf("p's num key = %d and leaf order = %d\n",p->num_key,leaf_order);

		leaf = insert_into_leaf(leaf, key, value);
		free(p);
		return 0;
	}

	//남은 자리가 없다면 쪼개야함
	free(p);

	insert_into_leaf_after_splitting(leaf, key, value);
	return 0;

	//그 키가 이미 있는지 확인한다.
	//없다면 새 레코드를 생성한다
	//만약 루트노드도 없다면 그 키로 루트를 새로 만들어준다
	//그 키가 들어갈 페이지를 가져온다
	//insert into leaf시전
	//리턴으로 leaf after split호출

}

int db_find(int64_t key, char * ret_val) {
	int i;
	//그 키가 들어있을수도 있는 리프페이지를 가져온다
	page_t * p = (page_t *)calloc(1, PAGESIZE);

	pagenum_t f_p = find_page(key);



	//빈트리인 경우
	if (f_p == -1) return -1;

	file_read_page(f_p, p);
	//키를 발견하면 멈춘다
	for (i = 0; i < p->num_key; i++)
	{
		if (p->record[i].key == key) break;
	}

	//인덱스가 끝번이 아니라면 찾지못한것 - break로 멈춰진게 아니라는 뜻
	if (i == p->num_key) {

		return -1;
	}
	else {


		strcpy(ret_val, p->record[i].val);

		return 0;
	}
	//만약 받아온 페이지가 null이라면 리턴종료
	//있다면 그 페이지 내에서 key와 일치하는 것을 찾는다
	//for 돌린후 키의 인덱스를 찾는다 i 5 
	//그 인덱스가 numkey와 같다면 못찾은것 --- -1리턴
	//아니라면 val를 저장하고  0이런
}

int db_delete(int64_t key) {


	char * tmp = (char *)malloc(120);
	pagenum_t leaf = find_page(key);

	//빈트리
	if (!(leaf > 0)) {
		return -1;
	}

	int ch = db_find(key, tmp);
	//찾았다면
	if (ch == 0) {
		delete_entry(leaf, key);
		free(tmp);
		return 0;
	}

	//못찾았다면 지울수없음
	else {
		free(tmp);
		return -1;
	}




}

int open_table(char *pathname) {


	//O_creat 모드에서는 세번째 인자에 파일에 접근권한 설정가능
	//파일이 없는 경우 새로 생성해주는데 파일이 존재한다면 에러를 리턴(O_EXCL)
	op_fd = open(pathname, O_RDWR | O_CREAT | O_EXCL, 0777);

	//페이지 크기만큼 할당해준다
	header_page * head;
	head = (header_page *)calloc(1, PAGESIZE);

	//정상적으로 열린경우 (파일이 존재하지 않아서 새로 생성한 경우)
	if (op_fd > 0) {

		//헤더 페이지 개수 카운트
		head->page_num = 1;
		head->free_page = 0;
		head->root_page = 0;

		//헤더 넣어주기
		file_write_page(0, (page_t *)head);
		file_read_page(0, (page_t *)head);
		return op_fd;
	}

	//파일이 이미 존재한다면 CREAT빼고 다시 읽어온다.
	op_fd = open(pathname, O_RDWR);





	if (op_fd > 0) {

		file_read_page(0, (page_t *)head);

		if (ch < PAGESIZE) {

			return -1;
		}


		return op_fd;
	}

	//다시 열었는데도 실패한경우는 열기 실패한 케이스
	printf("open failed\n");
	return -1;


}


///////////////

//나중에 지울부분


queue * q;
void print_tree() {
	printf("<print tree>\n");
	page_t * n;
	page_t * par_n;
	int i = 0;
	int rank = 0;
	int new_rank = 0;
	pagenum_t nnum, rnum;

	//header get
	header_page * head;
	head = (header_page *)calloc(1, PAGESIZE);
	file_read_page(0, (page_t *)head);

	//if  no root
	if (head->root_page == 0) {
		printf("empty tree.\n");
		return;
	}

	//printf("header info / pagenum %ld/ free page %ld/ root page %ld\n\n",head->page_num
	//,head->free_page,head->root_page);


	

	n = (page_t *)calloc(1, PAGESIZE);
	par_n = (page_t *)calloc(1, PAGESIZE);
	q = (queue*)malloc(sizeof(queue));
	q = NULL;
	rnum = head->root_page;
	enqueue(rnum);
	while (q != NULL) {
		//dequeue and trace
		nnum = dequeue();

		file_read_page(nnum, n);
		//printf("<my parent = %ld> <my pagenum = %ld> ",n->parent,nnum);
		for (i = 0; i < n->num_key; ++i) {
	
			printf("%" PRId64 " ", n->is_leaf ? n->record[i].key : n->branch[i].key);
		}
		//isnot leaf enqueue
		if (!n->is_leaf) {

			enqueue(n->right_left);
			for (i = 0; i < n->num_key; ++i)
				enqueue(n->branch[i].child);
		}
		printf("| ");
	}
	printf("\n");

	//print free page 
	page_t *  freeprint = (page_t *)calloc(1, PAGESIZE);
	file_read_page(head->free_page, freeprint);

	/*
	int cnt=0;
	pagenum_t tmp_next=head->free_page;
	if(head->free_page=0) printf("no free page\n");
	else{
	while(1){

		cnt++;
		printf("free %d : %ld \n",cnt,tmp_next);
		tmp_next = freeprint->parent;
		if(freeprint->parent==0)break;
		file_read_page(freeprint->parent,freeprint);


	}
	}
	*/
	free(n);
	free(par_n);
	free(q);

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
	printf("\n\n===please input instruction===\n\n");
	printf("1.insert(i <key> <val>)\n2.delete(d <key>)\n3.find(f <key>)\n4.print tree(t)\n5.quit(q)\n\n");



}




pagenum_t insert_into_new_root(pagenum_t left, int64_t key, pagenum_t right) {
	//페이지의 변경할 정보
	//리프/인터널 - numkey, {key, },parent, right_left
	//헤더 - 루트,프리,총개수

	pagenum_t new_root = make_internal_page();
	header_page * head = (header_page *)calloc(1, PAGESIZE);
	page_t * p=(page_t *) calloc(1, PAGESIZE);
	page_t * r= (page_t *)calloc(1, PAGESIZE);
	page_t * l= (page_t *)calloc(1, PAGESIZE);

	file_read_page(0, (page_t *)head);
	file_read_page(left, l);
	file_read_page(right, r);
	file_read_page(new_root, p);

	head->root_page = new_root;
	p->parent = 0;//루트 flag
	r->parent =new_root;
	l->parent = new_root;
	

	p->num_key++;
	p->right_left = left ;
	p->branch[0].key = key;
	p->branch[0].child = right;
	

	file_write_page(0, (page_t*)head);
	file_write_page(new_root, p);
	file_write_page(right, r);
	file_write_page(left, l);

	free(head);
	free(p);
	free(l);
	free(r);


	return new_root;

}


//처음 삽입
pagenum_t start_new_tree(int64_t key,char * val) {
	//페이지의 변경할 정보
	//리프/인터널 - numkey, {key, },parent, right_left
	//헤더 - 루트,프리,총개수


	pagenum_t leaf = make_leaf_page();

	page_t * l = (page_t *)calloc(1, PAGESIZE);
	file_read_page(leaf, l);

	header_page * head = (header_page *)calloc(1, PAGESIZE);
	file_read_page(0, (page_t *)head);


	head->root_page =leaf;
	head->page_num++;

	l->parent = 0;
	l->num_key = 1;
	

	
	
	l->record[0].key = key;

	//char *strcpy(char *dest, const char *src);
	//scr를 dest에 복사
	strcpy(l->record[0].val, val);
	//왜 l->record[0].val=val 가 되지 않는지
	//record의 var는 char, 받아온val는 char * 
	

	file_write_page(0, (page_t *)head);
	file_write_page(leaf, l);

	free(l);
	free(head);

	return 0;
}

pagenum_t find_page(int64_t key) {
	int i = 0;
	header_page * head= (header_page *)calloc(1, PAGESIZE);
	page_t * trace = (page_t *)calloc(1, PAGESIZE);
	//헤더를 받아온다

	file_read_page(0, (page_t *)head);

	//루트정보저장
	pagenum_t root = head->root_page;

	//빈트리 일경우

	if (root == 0) {

		free(trace);
		free(head);
		return -1;
	}

	//루트를 받아온다.

	file_read_page(root, trace);

	pagenum_t tmp=root;


	while (!trace->is_leaf) {

		i = 0;
		while (i < trace->num_key) {

			if (key >= trace->branch[i].key) {
				if(i==(trace->num_key -1)){
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

			tmp = trace->branch[i].child;

			file_read_page(trace->branch[i].child, trace);

		}
		else if (i == -1) {

			tmp = trace->right_left;

			file_read_page(tmp, trace);

		}
	
	}
	//while 이 끝났다는건 리프 까지 도착
	//이때 반환되는 리프 페이지에는 실제 키가 들어있을 수도 있고 없을 수도 있다
	//예를 들어 9를 찾는 경우, 7이상 10미만의 키들이 들어있어야하는 페이지가 반환되어도
	//거기에 9가 있다는 보장은 x

	free(head);
	free(trace);

	return tmp;
}


int cut(int length) {
	if (length % 2 == 0)
		return length / 2;
	else
		return length / 2 + 1;
}


// INSERTION//////////////////////////////////////////////

//여기서는 헤더정보 변경x ---alloc을 하면서 프리페이지 수정완료/총개수는 변함x
pagenum_t make_internal_page(void) {

	//페이지의 변경할 정보
	//리프/인터널 - numkey, {key, },parent, right_left, isleaf
	//헤더 - 루트,프리,총개수
	page_t *p = (page_t *)calloc(1,PAGESIZE);

	pagenum_t new_p=file_alloc_page();
	file_read_page(new_p, p);


	//값초기화 
	p->is_leaf = 0;
	p->num_key = 0;
	//아직 부모가 결정 안남
	p->parent = 0;
	//자기밑의 제일 왼쪽 페이지도 결정x
	p->right_left = 0;
	//키+offset또한 아직 설정할필요x

	file_write_page(new_p, p);
	free(p);

	return new_p;
}


pagenum_t make_leaf_page(void) {
	//페이지의 변경할 정보
	//리프/인터널 - numkey, {key, },parent, right_left
	//헤더 - 루트,프리,총개수

	pagenum_t leaf = make_internal_page();

	page_t * p=(page_t *) calloc(1, PAGESIZE);;
	file_read_page(leaf, p);

	p->is_leaf = 1;
	file_write_page(leaf, p);

	free(p);

	return leaf;
}
/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 삽입할 키 왼쪽에 있는 노드에 대한 부모 포인터의 인덱스를 찾기 위해 insert_into_parent에 사용되는 도우미 기능.
 */

int get_left_index(pagenum_t parent, pagenum_t left) {


	int i;
	page_t * p=(page_t *)calloc(1,PAGESIZE);
	page_t * l=(page_t *)calloc(1,PAGESIZE);

	file_read_page(parent, p);
	file_read_page(left, l);

	for (i = 0; i < p->num_key; i++)
	{
		if(p->branch[i].child==left)break;
	}

	free(p);
	free(l);

	return i;

}


//////////////////////////////////////////
pagenum_t insert_into_leaf(pagenum_t l, int64_t key, char * val) {
	page_t * leaf = (page_t *)calloc(1, PAGESIZE);
	file_read_page(l, leaf);
	int insertion_point=0;
	
	//삽입위치를 찾는다.
	while (insertion_point < leaf->num_key && leaf->record[insertion_point].key < key)
		insertion_point++;

	//삽입위치 자리를 밀어서 비워준다
	for (int i = leaf->num_key; i > insertion_point; i--) {
		leaf->record[i].key = leaf->record[i - 1].key;
		strcpy(leaf->record[i].val, leaf->record[i - 1].val);
		
	}

	//그 비워둔 위치에 삽입
	leaf->record[insertion_point].key = key;
	strcpy(leaf->record[insertion_point].val, val);
	leaf->num_key++;

	//수정된 leaf 다시 write하고 free
	file_write_page(l, leaf);
	free(leaf);

	return l;
}


pagenum_t insert_into_leaf_after_splitting(pagenum_t l, int64_t key, char * val) {


	//원래 있던 리프 만들어주기
	page_t * leaf = (page_t *)calloc(1, PAGESIZE);
	file_read_page(l, leaf);

	
	int insertion_index, split, new_key, i, j;

	//임시공간 마련
	key_val* tmp_record = (key_val *)malloc(33*sizeof(struct record));



	//쪼개서 넣어줄 새 리프 만들어주기
	pagenum_t n_p = make_leaf_page();
	page_t *new_leaf = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n_p, new_leaf);


	//tmp키포인터와 포인터를 가리키는 포인터를 만들어둔다 ? 왜 ---
	//그니까 원래3개만 들어갈수 있는데 하나가 더들어온경우 4개짜리를 보관할수있는 temp를 만들고
	//그걸 split해서 두개로 쪼개서 넣어준다는 말씀


	//삽입위치 찾기
	insertion_index = 0;
	while (insertion_index < leaf_order && leaf->record[insertion_index].key < key)
		insertion_index++;


	//삽입위치인경우 하나띄워서 leat의 값을 복사해서 가져와서 저장 
	for (i = 0, j = 0; i < leaf->num_key; i++, j++) {
		if (j == insertion_index) j++;
		tmp_record[j].key = leaf->record[i].key;
		strcpy(tmp_record[j].val, leaf->record[i].val);
	}
	//빈 삽입위치에 넣어주기
	tmp_record[insertion_index].key = key;
	strcpy(tmp_record[insertion_index].val, val);


	//원래 리프를 비우고 다시 채워준다
	leaf->num_key= 0;

	//분할지점 정하기
	split = cut(leaf_order);

	//원래 리프의 쪼갠부분의 앞쪽을 넣어주기
	for (i = 0; i < split; i++) {
		strcpy(leaf->record[i].val , tmp_record[i].val);
		leaf->record[i].key = tmp_record[i].key;
		leaf->num_key++;
	}
	//새 리프에 쪼갠부분의 나머지 부분을 넣어주기
	for (i = split, j = 0; i <= leaf_order; i++, j++) {
		strcpy(new_leaf->record[j].val , tmp_record[i].val);
		new_leaf->record[j].key = tmp_record[i].key;
		new_leaf->num_key++;
	}

	//tmp공간 free
	free(tmp_record);

	//새 리프랑 원래리프를 연결시켜준다  물론 그전에 원래리프가 가리키고 있던
	//다음 부분을 새로만든게 가리킬수 있도록 업뎃을 먼저 해둬야함
	new_leaf->right_left = leaf->right_left;
	leaf->right_left = n_p;

	//원래 리프를 재구성했으므로 원래값이 들어있던 부분들이 사라졌기때문에
	//거기에 대한 조정을 해준다
	for (i = leaf->num_key; i < leaf_order ; i++) {
		leaf->record[i].key = -1;
		
	}


	//값이 들은곳 뒤쪽부분 정리
	for (i = new_leaf->num_key; i < leaf_order ; i++) {
		new_leaf->record[i].key = -1;
		
	}

	//새로 만들어준 리프를 부모와 연결시켜주고 new키 설정 
	new_leaf->parent = leaf->parent;
	new_key = new_leaf->record[0].key;

	file_write_page(l,leaf);
	file_write_page(n_p,new_leaf);
	
	free(new_leaf);
	free(leaf);

	return insert_into_parent(l, new_key, n_p);
}


pagenum_t insert_into_node(pagenum_t n_p,int left_index, int64_t key, pagenum_t right) {
	int i;
	//n_p에 parent를 받아옴
	page_t * n = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n_p , n);

	//left 인덱스까지 한칸씩 오른쪽으로 미뤄서 자리를 확보한다
	for (i = n->num_key; i > left_index; i--) {
		n->branch[i + 1].child = n->branch[i].child;
		n->branch[i+1].key = n->branch[i].key;
	}

	//키와 포인터를 삽입
	n->branch[left_index + 1].child = right;
	n->branch[left_index+1].key = key;
	n->num_key++;
	
	file_write_page(n_p, n);

	free(n);
	return n_p;
}

pagenum_t insert_into_node_after_splitting(pagenum_t old, int left_index, int64_t key, pagenum_t right) {


	//into parent함수에서 old에 parent를 넘겨줌
	int i, j, split;
	int64_t k_prime;
	

	//원래 있던 노드 만들어주기
	page_t * old_node = (page_t *)calloc(1, PAGESIZE);
	file_read_page(old, old_node);

	//쪼개서 넣어줄 새 노드 만들어주기
	pagenum_t n_p = make_internal_page();
	page_t *new_node = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n_p, new_node);
	/* First create a temporary set of keys and pointers
	 * to hold everything in order, including
	 * the new key and pointer, inserted in their
	 * correct places.
	 * Then create a new node and copy half of the
	 * keys and pointers to the old node and
	 * the other half to the new.

	 먼저 새 키와 포인터를 포함한 모든 것을 올바른 위치에
	 삽입할 수 있는 임시 키와 포인터를 만드십시오.
	 그런 다음 새 노드를 생성하고 키와 포인터의 절반은 이전 노드에,
	 나머지 절반은 새 노드에 복사하십시오.
	 */


	 //임시공간 마련
	key_child * tmp_key_child = (key_child *)malloc(250 * sizeof(struct key_child));


	//삽입할 위치를 비워준다
	for (i = 0, j = 0; i < old_node->num_key + 1; i++, j++) {
		if (j == left_index + 1) j++;
		tmp_key_child[j].child = old_node->branch[i].child;
	}
	for (i = 0, j = 0; i < old_node->num_key; i++, j++) {
		if (j == left_index+1) j++;
		tmp_key_child[j].key = old_node->branch[i].key;
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
	old_node->num_key = 0;

	//원래있던 노드 채우기
	for (i = 0; i < split-1; i++) {
		old_node->num_key++;
		old_node->branch[i].key = tmp_key_child[i].key;
		old_node->branch[i].child= tmp_key_child[i].child;
		
	}

	//리프의 경우는 자신이 자식이 없기 때문에 그냥 올리는 작업을 했을때 문제x
	//그러나 인터널의 경우 자식을 갖고 올라가게 되므로
	//자신의 자식정보 수정이 필요


	k_prime = tmp_key_child[split-1].key;//올라갈 친구의 k_primed에 저장

	
	//새로만든 노드채우기
	new_node->num_key = 0;
	//split 지점 부터 넣음
	for (++i, j = 0; i <= node_order; i++, j++) {
		new_node->num_key++;
		new_node->branch[j].key = tmp_key_child[i].key;
		new_node->branch[j].child = tmp_key_child[i].child;
		
	}
	
	//올린 키가 원래 가리키고 있던 자식을 새로만든 노드의 right_left(rightmost)가 가리키게 설정
	new_node->right_left= tmp_key_child[split-1].child;

	//새로만든 노드 부모설정해주기
	new_node->parent = old_node->parent;


	//새로만든 노드의 자식의 부모설정도 마무리
	page_t * child = (page_t *)calloc(1, PAGESIZE);
	for (i = 0; i < new_node->num_key; i++) {
		file_read_page(new_node->branch[i].child, child);
		child->parent = n_p;
		file_write_page(new_node->branch[i].child, child);
	}
	file_read_page(new_node->right_left,child);
	child->parent =n_p;
	file_write_page(new_node->right_left,child);

	file_write_page(n_p,new_node);
	file_write_page(old, old_node);

	free(new_node);
	free(old_node);
	free(tmp_key_child);
	free(child);
	

	return insert_into_parent(old, k_prime, n_p);//올라갈 친구를 선정해서 부모에 삽입
}


pagenum_t insert_into_parent(pagenum_t left, int64_t key, pagenum_t right) {
	int left_index;
	pagenum_t parent;

	page_t * p = (page_t *)calloc(1, PAGESIZE);
	page_t * l = (page_t *)calloc(1, PAGESIZE);
	file_read_page(left, l);

	parent = l->parent;

	//새루트에 넣어야하는 경우(즉 left가 루트(internal)였던 경우)
	if (parent == 0) {

		free(l); free(p);
		return insert_into_new_root(left, key, right);
	}

	//아닌경우 부모를 받아온다
	file_read_page(parent, p);
	//left가 부모쪽에서 몇번째 인덱스에 있는지 확인

	left_index = get_left_index(parent, left);


	//부모페이지에 올려준다
	if (p->num_key < node_order) {

		free(l); free(p);
		return insert_into_node(parent, left_index, key, right);
	}
		
	 //부모로 하나 올렸는데 거기서도 꽉찬경우 다시 쪼개줘야함
	free(l); free(p);

	return insert_into_node_after_splitting(parent, left_index, key, right);
}





///////////////////////////////////////////////////////////////////////
// DELETION.////////////////////////////////////////////////////////
/*
삭제를 위한 유틸리티 기능. 노드의 가장 가까운 이웃(시블링) 인덱스가 있는 경우
 왼쪽을 검색하십시오.
 그렇지 않은 경우(노드는 가장 왼쪽의 하위 노드)이다. -1을 반환하여 이 특수 사례를 나타낸다.
 */

int get_neighbor_index(pagenum_t n_p) {
	page_t * n = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n_p, n);

	page_t * p = (page_t *)calloc(1, PAGESIZE);
	pagenum_t parent = n->parent;
	file_read_page(parent, p);
	int i;

	/*
	키의 인덱스를 n_p을 가리키는 부모에 있는 포인터의 왼쪽인덱스으로 반환.
	 n이 가장 왼쪽의 아이일 경우, 이는 -2이 돌아온다는 것을 의미한다.
	 
	*/

	if (p->right_left == n_p) return -2;

	for (i = 0; i < p->num_key; i++) {
		if (p->branch[i].child == n_p) {
			//이때 i는 부모에게서 받아온 자기인덱스
			free(n); free(p);
			return i - 1;//즉이건 부모쪽에서 확인한 key의 바로 왼쪽형제 인덱스
		}
	}

}

pagenum_t remove_entry_from_node(pagenum_t n, int64_t key) {
	int i;
	
	page_t * node  = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n, node);

	// Remove the key and shift other keys accordingly.
	i = 0;
	//키위치 찾고 키를 당겨오면서 덮어씌워준다
	//internal인경우
	if(!node->is_leaf) {

		while (node->branch[i].key != key)
			i++;
		//키랑 child  당겨주기
		for (++i; i < node->num_key; i++) {

			node->branch[i - 1].child=node->branch[i].child;
			node->branch[i - 1].key = node->branch[i].key;

		}
	}
	//리프인경우
	else if (node->is_leaf) {

		while (node->record[i].key != key)
			i++;

		//키랑 val 당겨주기
		for (++i; i < node->num_key; i++) {

			strcpy(node->record[i - 1].val, node->record[i].val);
			node->record[i - 1].key = node->record[i].key;

		}
	}



	// Remove the pointer and shift other pointers accordingly.
	// First determine number of pointers.

	// One key fewer.
	node->num_key--;

	// Set the other pointers to NULL for tidiness.//정돈을 위해

	if (node->is_leaf) {

		for (i = node->num_key; i < leaf_order; i++) {
			node->record[i].key = -1;
			node->record[i].val[0] = '\0';
		}
			
	}
	else {

		for (i = node->num_key; i < node_order; i++) {
			node->branch[i].child = -1;
			node->branch[i].key = -1;
		}

	}

	file_write_page(n, node);

	free(node);

	return n;
}


pagenum_t adjust_root(pagenum_t r) {
	//지운게 루트노드에 있을때의 조절과정임 - 이미 지우기는 끝난상태
	page_t * root = (page_t *)calloc(1, PAGESIZE);
	file_read_page(r, root);

	page_t * new_root = (page_t *)calloc(1, PAGESIZE);
	file_read_page(root->right_left, new_root);
	
	header_page * head = (header_page *)calloc(1, PAGESIZE);
	file_read_page(0, (page_t *)head);

	/* Case: nonempty root.
	키랑 포인터가 이미 지워져있고 할게 없는 경우
	 * Key and pointer have already been deleted,
	 * so nothing to be done.
	 */
	 //비어있지 않은 루트 조절-- 그니까 이전에 삭제를 했는데 이후의 루트가 비어있지 않으면 별문제
	 //없으므로 그냥 끝이라는 소리
	if (root->num_key > 0) {
		free(new_root); free(root); free(head);
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
	if (!root->is_leaf) {

		file_free_page(r);
		head->root_page = root->right_left;
		new_root->parent = 0;
		file_write_page(root->right_left, new_root);
	
	
		
	}
	//
	// If it is a leaf (has no children),
	// then the whole tree is empty.

	//루트가 리프일때 --- child가 없을때 즉이건 모든 트리가 비어있다는 것
	//루트페이지를 지우면됌
	else {

		file_free_page(r);
		head->root_page = 0;
	
		
	}

	file_write_page(0, (page_t *)head);
	pagenum_t n_r = head->root_page;
	free(root); free(head); free(new_root);

	//새로 조정된루트
	return n_r;
}

pagenum_t coalesce_nodes(pagenum_t n, pagenum_t n_p, int neighbor_index, int64_t k_prime) {
	//받아온 kprime은 지워질노드의 부모에서의 키
	//받아온 이웃 인덱스는 찐인덱스아님 -2 있음
	//n_p는 찐 이웃
	int i, j, neighbor_insertion_index, n_end;
	pagenum_t tmp;

	page_t * t_p = (page_t *)calloc(1, PAGESIZE);
	page_t * node = (page_t *)calloc(1, PAGESIZE);//없어질 노드
	page_t * neighbor = (page_t *)calloc(1, PAGESIZE);//합쳐서 모을 노드
	
	pagenum_t change_neighbor;
	pagenum_t change_die;

	 //현재노드가 제일 왼쪽이면 오른쪽노드랑 합친다
	//대신 오른쪽 노드에 모이는게 아니고 여전히 왼쪽으로 모인다.
	//그리고 오른쪽 노드를 삭제하는 것임
	if (neighbor_index == -2) {

		//nei안에는 지워질 
		file_read_page(n_p, node);
		file_read_page(n, neighbor);
		change_neighbor = n;
		change_die=n_p;
	}
	else {

		file_read_page(n, node);
		file_read_page(n_p,neighbor);
		change_neighbor = n_p;
		change_die=n;
	}
	pagenum_t parent = node->parent;
	page_t * p = (page_t *)calloc(1, PAGESIZE);
	file_read_page(parent, p);


	/* 
	 n에서 키 및 포인터를 복사하기 위한 인접 노드의 시작점.
	 n과 이웃이 n이 가장 왼쪽의 자식인 경우에 자리를 바꾸었다는 것을 기억.
	 */

	neighbor_insertion_index = neighbor->num_key;

	 //리프가 아닌경우
	if (!node->is_leaf) {

		neighbor->branch[neighbor_insertion_index].key = k_prime;
		neighbor->branch[neighbor_insertion_index].child = node->right_left;
		neighbor->num_key++;
		//ok

		
		
		//이웃노드로 옮겨주는 과정
		for (i = neighbor_insertion_index + 1, j = 0; j < node->num_key; i++, j++) {
			neighbor->branch[i].key = node->branch[j].key;
			neighbor->branch[i].child = node->branch[j].child;
			neighbor->num_key++;
			
		}

		//왼쪽으로 다 옮겨왔으므로 부모가 모두 동일할것임-- 부모수정은 x

		 //현재 n의 자식들도  neighbor를 가리키게 변경해줌
		for (i = 0; i < neighbor->num_key ; i++) {
			tmp = neighbor->branch[i].child;
			file_read_page(tmp, t_p);
			t_p->parent =change_neighbor;
			file_write_page(tmp, t_p);
		}

		tmp = node->right_left;
		file_read_page(tmp,t_p);

		t_p->parent = change_neighbor;

		file_write_page(tmp,t_p);

		
		file_free_page(change_die);

		free(t_p);

	}

	//리프인경우
	/*
	 key가 0개 이므로 바로free ,그리고 올바른 right page를 가리키게 재설정
	 */

	else {

		if(neighbor_index != -2){

		neighbor->right_left = node->right_left;
		file_free_page(change_die);
		}
		else {//kprime is parent's index 0's key


		//이웃노드로 옮겨주는 과정
		for (i = neighbor_insertion_index, j = 0; j < node->num_key; i++, j++) {



			neighbor->record[i].key = node->record[j].key;
			strcpy(neighbor->record[i].val, node->record[j].val);
			neighbor->num_key++;
			
		}
		neighbor->right_left=node->right_left;
		file_free_page(change_die);
		
		}
		

		
	}

	//n의 흔적을 삭제시켜준다

	file_write_page(change_neighbor,neighbor);
	delete_entry(parent, k_prime);

	free(p);
	free(neighbor);
	
	free(node);

	return n;
}

pagenum_t redistribute_nodes(pagenum_t n, pagenum_t n_p, int neighbor_index, int k_prime_index, int64_t k_prime) {

	//delay merge로인해  node는 절대 leaf일 수 x
	pagenum_t tmp;
	int i;
	page_t * t_p = (page_t *)calloc(1, PAGESIZE);
	page_t * node = (page_t *)calloc(1, PAGESIZE);//없어질 노드
	page_t * neighbor = (page_t *)calloc(1, PAGESIZE);//합쳐서 모을 노드

	file_read_page(n,node);
	file_read_page(n_p,neighbor);
	pagenum_t parent = node->parent;
	page_t * p = (page_t *)calloc(1, PAGESIZE);
	file_read_page(parent, p);


	/* Case: n has a neighbor to the left.
		이웃의 마지막 키-점퍼 쌍을 이웃의 오른쪽 끝에서 n의 왼쪽 끝으로 당기십시오.
	그니까 이건 '이웃 - 나' 이렇게 있으면 이웃꺼 하나를 내꺼로 당겨오는거임
	즉 합치면 넘치니까 합치는 대신 빌려오는 거
	 */

	if (neighbor_index != -2) {

		//리프가 아닌경우 
		//대신이때 리프가 아니라는건 자식도 있다는 소리니까 자식들도 옮겨주는 작업필요 - tmp
		if (!node->is_leaf) {
			//일단 앞에 한자리 비워야되니까 한칸씩 쭉밀어준다
		

			//child포인터들을 이동
			node->branch[0].child = node->right_left;
			node->right_left = neighbor->branch[neighbor->num_key - 1].child;
			
			//빌려온 자식의 부모설정을 다시
			tmp = node->right_left;
			file_read_page(tmp, t_p);
			t_p->parent = n;
			file_write_page(tmp, t_p);
			
			//이웃의 키하나를 keyprime index에 부모로 올리고
			//그자리에서 부모의 key prime을 빌려온다(아이패드 그림 참조)
			node->branch[0].key = k_prime;
			p->branch[k_prime_index].key = neighbor->branch[neighbor->num_key - 1].key;

			//이웃쪽은 뒤에 한칸을 초기화
			neighbor->branch[neighbor->num_key - 1].child = -1;
			neighbor->branch[neighbor->num_key - 1].key = -1;

			
		}

		//리프인경우 -  비워둔 자리에 옮겨준다
		//자식이 없으므로 신경쓸필요 없다
		else {
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
		//리프인경우 - 밑에 자식들에 대한 조정 필요x
		if (node->is_leaf) {
			//원래라면 삭제후 최소크기를 만족못한 리프가 이웃으로 합쳐지고
			//합쳐졌을때 capacity를 초과하게되서 이함수가 불리는데 
			//이번의 경우 합쳐질 리프가 numkey가 0이므로 초과가 불가능
			//즉 이 함수 안에서는 leaf에 대한 고려는 하지 않아도 됨
		}

		//리프가 아닌경우 밑에자식들에 대한 조정도 필요
		else {
			//부모쪽에서 keyprime을 갖고오고 이웃의 child pointer를 가져온다
			node->branch[0].key = k_prime;
			node->branch[0].child = neighbor->right_left;
			neighbor->right_left = neighbor->branch[0].child;
		
			tmp = node->branch[0].child;
			file_read_page(tmp, t_p);
			t_p->parent = n;
			p->branch[k_prime_index].key = neighbor->branch[0].key;
			file_write_page(tmp, t_p);


			//이웃은 지금 앞의 한칸이 비워져있으므로 당겨와야함
			for (i = 0; i < neighbor->num_key-1; i++) {
				neighbor->branch[i].key = neighbor->branch[i + 1].key;
				neighbor->branch[i].child = neighbor->branch[i + 1].child;
			}
			neighbor->branch[i].key=-1;
			neighbor->branch[i].child=-1;
		}
	}


	/*
	 n은 이제 한 개의 열쇠와 한 개의 포인터를 더 가지고 있으며,
	 이웃은 각각 한 개씩 더 적게 가지고 있다.
	 그니까 하나 빌려왔다 이말
	 */

	node->num_key++;
	neighbor->num_key--;

	file_write_page(n, node);
	file_write_page(n_p, neighbor);
	file_write_page(parent, p);
	

	free(node);
	free(neighbor);
	free(p);
	free(t_p);


	return n;
}

pagenum_t delete_entry(pagenum_t n, int64_t key) {

	//받아온 페이지에서 그 키를 삭제


	int neighbor_index;
	int k_prime_index;
	int64_t k_prime;
	int capacity;

	page_t * node = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n, node);

	pagenum_t parent = node->parent;
	page_t * p = (page_t *)calloc(1, PAGESIZE);
	file_read_page(parent, p);

	header_page * head = (header_page *)calloc(1, PAGESIZE);
	file_read_page(0, (page_t *)head);

	//그 페이지에서 키를 지운다 

	remove_entry_from_node(n, key);
	file_read_page(n,node);


	//지운노드 루트노드였을 때 
	if (n == head->root_page) {

		file_write_page(n,node);
		free(node); free(p); free(head);
		
		return adjust_root(n);
	}

	/*
	 삭제 후 보존할 노드의 최소 허용 크기를 결정한다.근데 우리는 delay merge를 할거니까 필요x
	 키 개수가 0 이상이라면 그냥 merge할필요 x
	 */

	if (node->num_key > 0) {

		file_write_page(n,node);

		free(node); free(p); free(head);
		return n;
	}

	//key개수가 0이 된경우

	//이웃을 받아온다
	neighbor_index = get_neighbor_index(n);


	//key프라임 결정--- 아마 들어가야
	//-1이라는건 왼쪽이웃이 없어서 오른쪽 이웃을 택하는경우
	//여기서 키프라임인덱스는 사라질 키의 부모쪽 인덱스
	//맨왼쪽의 경우는 -1번째 키가 없기때문에 0번째 키를 삭제시켜서 
	//해당 인덱스의 child가 right_left로 들어갈 수 있게 해준다.
	k_prime_index = neighbor_index == -2 ? 0 : neighbor_index + 1;
	//키프라임은 위의 받아온 인덱스를 갖고 부모쪽에 저장된 내 키갖고 오기
	//대신 내가 맨오른쪽이라면 자신의 키가 없으므로 가장처음 키인 0번째키로 설정
	k_prime = p->record[k_prime_index].key;

	//-2이라면 자신이 제일 왼쪽 이웃이 없으므로 오른쪽에 옮긴다
	int real_nei=neighbor_index;
	if (neighbor_index == -2)real_nei = 0;

	page_t * neighbor = (page_t *)calloc(1, PAGESIZE);
	pagenum_t nei_p = p->branch[real_nei].child;
	file_read_page(nei_p, neighbor);

	capacity = node->is_leaf ? leaf_order : node_order;

	//찐이웃인덱스 안넘겨줌 -2로 판별가능하게 넘겨줌
	/* Coalescence. */
	//이경우는 합쳤는데 개수 ok일때
	if (neighbor->num_key + node->num_key < capacity) {

		file_write_page(n,node);
		free(neighbor); free(node); free(p); free(head);
		return coalesce_nodes(n, nei_p, neighbor_index, k_prime);
	}

	/* Redistribution. */
	//합쳣는데 개수 초과로 재조정
	else
	{

		file_write_page(n,node);
		free(neighbor); free(node); free(p); free(head);
		return redistribute_nodes(n, nei_p, neighbor_index, k_prime_index, k_prime);
	}
}


