


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


	//ã�� ��� -- �̹� �ִ� ���

//find case--- duplicate
	if (!db_find(key, value)) {
		//printf("db insert find\n");
		return -1;
	}

	//��ã����� -- ���� �� ����
	//Ʈ���� �ȸ������ �ִ°��(ù ������ ���)
	if (head->root_page == 0) {
		//	printf("db insert root page\n");
		return start_new_tree(key, value);
	}

	//Ʈ���� ��������ִ� ���
	//Ű�� �� ���������� �о���δ�
	page_t * p = (page_t *)calloc(1, PAGESIZE);
	pagenum_t leaf = find_page(key);
	file_read_page(leaf, p);

	//printf("db insert here -- leaf num = %ld\n",leaf);


	//���� �������� ���� �ڸ��� �ִ� ���
	if (p->num_key < leaf_order) {
		//printf("db insert-insert into leaf call\n");
//printf("p's num key = %d and leaf order = %d\n",p->num_key,leaf_order);

		leaf = insert_into_leaf(leaf, key, value);
		free(p);
		return 0;
	}

	//���� �ڸ��� ���ٸ� �ɰ�����
	free(p);

	insert_into_leaf_after_splitting(leaf, key, value);
	return 0;

	//�� Ű�� �̹� �ִ��� Ȯ���Ѵ�.
	//���ٸ� �� ���ڵ带 �����Ѵ�
	//���� ��Ʈ��嵵 ���ٸ� �� Ű�� ��Ʈ�� ���� ������ش�
	//�� Ű�� �� �������� �����´�
	//insert into leaf����
	//�������� leaf after splitȣ��

}

int db_find(int64_t key, char * ret_val) {
	int i;
	//�� Ű�� ����������� �ִ� ������������ �����´�
	page_t * p = (page_t *)calloc(1, PAGESIZE);

	pagenum_t f_p = find_page(key);



	//��Ʈ���� ���
	if (f_p == -1) return -1;

	file_read_page(f_p, p);
	//Ű�� �߰��ϸ� �����
	for (i = 0; i < p->num_key; i++)
	{
		if (p->record[i].key == key) break;
	}

	//�ε����� ������ �ƴ϶�� ã�����Ѱ� - break�� �������� �ƴ϶�� ��
	if (i == p->num_key) {

		return -1;
	}
	else {


		strcpy(ret_val, p->record[i].val);

		return 0;
	}
	//���� �޾ƿ� �������� null�̶�� ��������
	//�ִٸ� �� ������ ������ key�� ��ġ�ϴ� ���� ã�´�
	//for ������ Ű�� �ε����� ã�´� i 5 
	//�� �ε����� numkey�� ���ٸ� ��ã���� --- -1����
	//�ƴ϶�� val�� �����ϰ�  0�̷�
}

int db_delete(int64_t key) {


	char * tmp = (char *)malloc(120);
	pagenum_t leaf = find_page(key);

	//��Ʈ��
	if (!(leaf > 0)) {
		return -1;
	}

	int ch = db_find(key, tmp);
	//ã�Ҵٸ�
	if (ch == 0) {
		delete_entry(leaf, key);
		free(tmp);
		return 0;
	}

	//��ã�Ҵٸ� ���������
	else {
		free(tmp);
		return -1;
	}




}

int open_table(char *pathname) {


	//O_creat ��忡���� ����° ���ڿ� ���Ͽ� ���ٱ��� ��������
	//������ ���� ��� ���� �������ִµ� ������ �����Ѵٸ� ������ ����(O_EXCL)
	op_fd = open(pathname, O_RDWR | O_CREAT | O_EXCL, 0777);

	//������ ũ�⸸ŭ �Ҵ����ش�
	header_page * head;
	head = (header_page *)calloc(1, PAGESIZE);

	//���������� ������� (������ �������� �ʾƼ� ���� ������ ���)
	if (op_fd > 0) {

		//��� ������ ���� ī��Ʈ
		head->page_num = 1;
		head->free_page = 0;
		head->root_page = 0;

		//��� �־��ֱ�
		file_write_page(0, (page_t *)head);
		file_read_page(0, (page_t *)head);
		return op_fd;
	}

	//������ �̹� �����Ѵٸ� CREAT���� �ٽ� �о�´�.
	op_fd = open(pathname, O_RDWR);





	if (op_fd > 0) {

		file_read_page(0, (page_t *)head);

		if (ch < PAGESIZE) {

			return -1;
		}


		return op_fd;
	}

	//�ٽ� �����µ��� �����Ѱ��� ���� ������ ���̽�
	printf("open failed\n");
	return -1;


}


///////////////

//���߿� ����κ�


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
	//�������� ������ ����
	//����/���ͳ� - numkey, {key, },parent, right_left
	//��� - ��Ʈ,����,�Ѱ���

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
	p->parent = 0;//��Ʈ flag
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


//ó�� ����
pagenum_t start_new_tree(int64_t key,char * val) {
	//�������� ������ ����
	//����/���ͳ� - numkey, {key, },parent, right_left
	//��� - ��Ʈ,����,�Ѱ���


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
	//scr�� dest�� ����
	strcpy(l->record[0].val, val);
	//�� l->record[0].val=val �� ���� �ʴ���
	//record�� var�� char, �޾ƿ�val�� char * 
	

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
	//����� �޾ƿ´�

	file_read_page(0, (page_t *)head);

	//��Ʈ��������
	pagenum_t root = head->root_page;

	//��Ʈ�� �ϰ��

	if (root == 0) {

		free(trace);
		free(head);
		return -1;
	}

	//��Ʈ�� �޾ƿ´�.

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
	//while �� �����ٴ°� ���� ���� ����
	//�̶� ��ȯ�Ǵ� ���� ���������� ���� Ű�� ������� ���� �ְ� ���� ���� �ִ�
	//���� ��� 9�� ã�� ���, 7�̻� 10�̸��� Ű���� ����־���ϴ� �������� ��ȯ�Ǿ
	//�ű⿡ 9�� �ִٴ� ������ x

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

//���⼭�� ������� ����x ---alloc�� �ϸ鼭 ���������� �����Ϸ�/�Ѱ����� ����x
pagenum_t make_internal_page(void) {

	//�������� ������ ����
	//����/���ͳ� - numkey, {key, },parent, right_left, isleaf
	//��� - ��Ʈ,����,�Ѱ���
	page_t *p = (page_t *)calloc(1,PAGESIZE);

	pagenum_t new_p=file_alloc_page();
	file_read_page(new_p, p);


	//���ʱ�ȭ 
	p->is_leaf = 0;
	p->num_key = 0;
	//���� �θ� ���� �ȳ�
	p->parent = 0;
	//�ڱ���� ���� ���� �������� ����x
	p->right_left = 0;
	//Ű+offset���� ���� �������ʿ�x

	file_write_page(new_p, p);
	free(p);

	return new_p;
}


pagenum_t make_leaf_page(void) {
	//�������� ������ ����
	//����/���ͳ� - numkey, {key, },parent, right_left
	//��� - ��Ʈ,����,�Ѱ���

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
 ������ Ű ���ʿ� �ִ� ��忡 ���� �θ� �������� �ε����� ã�� ���� insert_into_parent�� ���Ǵ� ����� ���.
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
	
	//������ġ�� ã�´�.
	while (insertion_point < leaf->num_key && leaf->record[insertion_point].key < key)
		insertion_point++;

	//������ġ �ڸ��� �о ����ش�
	for (int i = leaf->num_key; i > insertion_point; i--) {
		leaf->record[i].key = leaf->record[i - 1].key;
		strcpy(leaf->record[i].val, leaf->record[i - 1].val);
		
	}

	//�� ����� ��ġ�� ����
	leaf->record[insertion_point].key = key;
	strcpy(leaf->record[insertion_point].val, val);
	leaf->num_key++;

	//������ leaf �ٽ� write�ϰ� free
	file_write_page(l, leaf);
	free(leaf);

	return l;
}


pagenum_t insert_into_leaf_after_splitting(pagenum_t l, int64_t key, char * val) {


	//���� �ִ� ���� ������ֱ�
	page_t * leaf = (page_t *)calloc(1, PAGESIZE);
	file_read_page(l, leaf);

	
	int insertion_index, split, new_key, i, j;

	//�ӽð��� ����
	key_val* tmp_record = (key_val *)malloc(33*sizeof(struct record));



	//�ɰ��� �־��� �� ���� ������ֱ�
	pagenum_t n_p = make_leaf_page();
	page_t *new_leaf = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n_p, new_leaf);


	//tmpŰ�����Ϳ� �����͸� ����Ű�� �����͸� �����д� ? �� ---
	//�״ϱ� ����3���� ���� �ִµ� �ϳ��� �����°�� 4��¥���� �����Ҽ��ִ� temp�� �����
	//�װ� split�ؼ� �ΰ��� �ɰ��� �־��شٴ� ����


	//������ġ ã��
	insertion_index = 0;
	while (insertion_index < leaf_order && leaf->record[insertion_index].key < key)
		insertion_index++;


	//������ġ�ΰ�� �ϳ������ leat�� ���� �����ؼ� �����ͼ� ���� 
	for (i = 0, j = 0; i < leaf->num_key; i++, j++) {
		if (j == insertion_index) j++;
		tmp_record[j].key = leaf->record[i].key;
		strcpy(tmp_record[j].val, leaf->record[i].val);
	}
	//�� ������ġ�� �־��ֱ�
	tmp_record[insertion_index].key = key;
	strcpy(tmp_record[insertion_index].val, val);


	//���� ������ ���� �ٽ� ä���ش�
	leaf->num_key= 0;

	//�������� ���ϱ�
	split = cut(leaf_order);

	//���� ������ �ɰ��κ��� ������ �־��ֱ�
	for (i = 0; i < split; i++) {
		strcpy(leaf->record[i].val , tmp_record[i].val);
		leaf->record[i].key = tmp_record[i].key;
		leaf->num_key++;
	}
	//�� ������ �ɰ��κ��� ������ �κ��� �־��ֱ�
	for (i = split, j = 0; i <= leaf_order; i++, j++) {
		strcpy(new_leaf->record[j].val , tmp_record[i].val);
		new_leaf->record[j].key = tmp_record[i].key;
		new_leaf->num_key++;
	}

	//tmp���� free
	free(tmp_record);

	//�� ������ ���������� ��������ش�  ���� ������ ���������� ����Ű�� �ִ�
	//���� �κ��� ���θ���� ����ų�� �ֵ��� ������ ���� �ص־���
	new_leaf->right_left = leaf->right_left;
	leaf->right_left = n_p;

	//���� ������ �籸�������Ƿ� �������� ����ִ� �κе��� ������⶧����
	//�ű⿡ ���� ������ ���ش�
	for (i = leaf->num_key; i < leaf_order ; i++) {
		leaf->record[i].key = -1;
		
	}


	//���� ������ ���ʺκ� ����
	for (i = new_leaf->num_key; i < leaf_order ; i++) {
		new_leaf->record[i].key = -1;
		
	}

	//���� ������� ������ �θ�� ��������ְ� newŰ ���� 
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
	//n_p�� parent�� �޾ƿ�
	page_t * n = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n_p , n);

	//left �ε������� ��ĭ�� ���������� �̷Ｍ �ڸ��� Ȯ���Ѵ�
	for (i = n->num_key; i > left_index; i--) {
		n->branch[i + 1].child = n->branch[i].child;
		n->branch[i+1].key = n->branch[i].key;
	}

	//Ű�� �����͸� ����
	n->branch[left_index + 1].child = right;
	n->branch[left_index+1].key = key;
	n->num_key++;
	
	file_write_page(n_p, n);

	free(n);
	return n_p;
}

pagenum_t insert_into_node_after_splitting(pagenum_t old, int left_index, int64_t key, pagenum_t right) {


	//into parent�Լ����� old�� parent�� �Ѱ���
	int i, j, split;
	int64_t k_prime;
	

	//���� �ִ� ��� ������ֱ�
	page_t * old_node = (page_t *)calloc(1, PAGESIZE);
	file_read_page(old, old_node);

	//�ɰ��� �־��� �� ��� ������ֱ�
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

	 ���� �� Ű�� �����͸� ������ ��� ���� �ùٸ� ��ġ��
	 ������ �� �ִ� �ӽ� Ű�� �����͸� ����ʽÿ�.
	 �׷� ���� �� ��带 �����ϰ� Ű�� �������� ������ ���� ��忡,
	 ������ ������ �� ��忡 �����Ͻʽÿ�.
	 */


	 //�ӽð��� ����
	key_child * tmp_key_child = (key_child *)malloc(250 * sizeof(struct key_child));


	//������ ��ġ�� ����ش�
	for (i = 0, j = 0; i < old_node->num_key + 1; i++, j++) {
		if (j == left_index + 1) j++;
		tmp_key_child[j].child = old_node->branch[i].child;
	}
	for (i = 0, j = 0; i < old_node->num_key; i++, j++) {
		if (j == left_index+1) j++;
		tmp_key_child[j].key = old_node->branch[i].key;
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
	old_node->num_key = 0;

	//�����ִ� ��� ä���
	for (i = 0; i < split-1; i++) {
		old_node->num_key++;
		old_node->branch[i].key = tmp_key_child[i].key;
		old_node->branch[i].child= tmp_key_child[i].child;
		
	}

	//������ ���� �ڽ��� �ڽ��� ���� ������ �׳� �ø��� �۾��� ������ ����x
	//�׷��� ���ͳ��� ��� �ڽ��� ���� �ö󰡰� �ǹǷ�
	//�ڽ��� �ڽ����� ������ �ʿ�


	k_prime = tmp_key_child[split-1].key;//�ö� ģ���� k_primed�� ����

	
	//���θ��� ���ä���
	new_node->num_key = 0;
	//split ���� ���� ����
	for (++i, j = 0; i <= node_order; i++, j++) {
		new_node->num_key++;
		new_node->branch[j].key = tmp_key_child[i].key;
		new_node->branch[j].child = tmp_key_child[i].child;
		
	}
	
	//�ø� Ű�� ���� ����Ű�� �ִ� �ڽ��� ���θ��� ����� right_left(rightmost)�� ����Ű�� ����
	new_node->right_left= tmp_key_child[split-1].child;

	//���θ��� ��� �θ������ֱ�
	new_node->parent = old_node->parent;


	//���θ��� ����� �ڽ��� �θ����� ������
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
	

	return insert_into_parent(old, k_prime, n_p);//�ö� ģ���� �����ؼ� �θ� ����
}


pagenum_t insert_into_parent(pagenum_t left, int64_t key, pagenum_t right) {
	int left_index;
	pagenum_t parent;

	page_t * p = (page_t *)calloc(1, PAGESIZE);
	page_t * l = (page_t *)calloc(1, PAGESIZE);
	file_read_page(left, l);

	parent = l->parent;

	//����Ʈ�� �־���ϴ� ���(�� left�� ��Ʈ(internal)���� ���)
	if (parent == 0) {

		free(l); free(p);
		return insert_into_new_root(left, key, right);
	}

	//�ƴѰ�� �θ� �޾ƿ´�
	file_read_page(parent, p);
	//left�� �θ��ʿ��� ���° �ε����� �ִ��� Ȯ��

	left_index = get_left_index(parent, left);


	//�θ��������� �÷��ش�
	if (p->num_key < node_order) {

		free(l); free(p);
		return insert_into_node(parent, left_index, key, right);
	}
		
	 //�θ�� �ϳ� �÷ȴµ� �ű⼭�� ������� �ٽ� �ɰ������
	free(l); free(p);

	return insert_into_node_after_splitting(parent, left_index, key, right);
}





///////////////////////////////////////////////////////////////////////
// DELETION.////////////////////////////////////////////////////////
/*
������ ���� ��ƿ��Ƽ ���. ����� ���� ����� �̿�(�ú�) �ε����� �ִ� ���
 ������ �˻��Ͻʽÿ�.
 �׷��� ���� ���(���� ���� ������ ���� ���)�̴�. -1�� ��ȯ�Ͽ� �� Ư�� ��ʸ� ��Ÿ����.
 */

int get_neighbor_index(pagenum_t n_p) {
	page_t * n = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n_p, n);

	page_t * p = (page_t *)calloc(1, PAGESIZE);
	pagenum_t parent = n->parent;
	file_read_page(parent, p);
	int i;

	/*
	Ű�� �ε����� n_p�� ����Ű�� �θ� �ִ� �������� �����ε������� ��ȯ.
	 n�� ���� ������ ������ ���, �̴� -2�� ���ƿ´ٴ� ���� �ǹ��Ѵ�.
	 
	*/

	if (p->right_left == n_p) return -2;

	for (i = 0; i < p->num_key; i++) {
		if (p->branch[i].child == n_p) {
			//�̶� i�� �θ𿡰Լ� �޾ƿ� �ڱ��ε���
			free(n); free(p);
			return i - 1;//���̰� �θ��ʿ��� Ȯ���� key�� �ٷ� �������� �ε���
		}
	}

}

pagenum_t remove_entry_from_node(pagenum_t n, int64_t key) {
	int i;
	
	page_t * node  = (page_t *)calloc(1, PAGESIZE);
	file_read_page(n, node);

	// Remove the key and shift other keys accordingly.
	i = 0;
	//Ű��ġ ã�� Ű�� ��ܿ��鼭 ������ش�
	//internal�ΰ��
	if(!node->is_leaf) {

		while (node->branch[i].key != key)
			i++;
		//Ű�� child  ����ֱ�
		for (++i; i < node->num_key; i++) {

			node->branch[i - 1].child=node->branch[i].child;
			node->branch[i - 1].key = node->branch[i].key;

		}
	}
	//�����ΰ��
	else if (node->is_leaf) {

		while (node->record[i].key != key)
			i++;

		//Ű�� val ����ֱ�
		for (++i; i < node->num_key; i++) {

			strcpy(node->record[i - 1].val, node->record[i].val);
			node->record[i - 1].key = node->record[i].key;

		}
	}



	// Remove the pointer and shift other pointers accordingly.
	// First determine number of pointers.

	// One key fewer.
	node->num_key--;

	// Set the other pointers to NULL for tidiness.//������ ����

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
	//����� ��Ʈ��忡 �������� ���������� - �̹� ������ ��������
	page_t * root = (page_t *)calloc(1, PAGESIZE);
	file_read_page(r, root);

	page_t * new_root = (page_t *)calloc(1, PAGESIZE);
	file_read_page(root->right_left, new_root);
	
	header_page * head = (header_page *)calloc(1, PAGESIZE);
	file_read_page(0, (page_t *)head);

	/* Case: nonempty root.
	Ű�� �����Ͱ� �̹� �������ְ� �Ұ� ���� ���
	 * Key and pointer have already been deleted,
	 * so nothing to be done.
	 */
	 //������� ���� ��Ʈ ����-- �״ϱ� ������ ������ �ߴµ� ������ ��Ʈ�� ������� ������ ������
	 //�����Ƿ� �׳� ���̶�� �Ҹ�
	if (root->num_key > 0) {
		free(new_root); free(root); free(head);
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
	if (!root->is_leaf) {

		file_free_page(r);
		head->root_page = root->right_left;
		new_root->parent = 0;
		file_write_page(root->right_left, new_root);
	
	
		
	}
	//
	// If it is a leaf (has no children),
	// then the whole tree is empty.

	//��Ʈ�� �����϶� --- child�� ������ ���̰� ��� Ʈ���� ����ִٴ� ��
	//��Ʈ�������� ������
	else {

		file_free_page(r);
		head->root_page = 0;
	
		
	}

	file_write_page(0, (page_t *)head);
	pagenum_t n_r = head->root_page;
	free(root); free(head); free(new_root);

	//���� �����ȷ�Ʈ
	return n_r;
}

pagenum_t coalesce_nodes(pagenum_t n, pagenum_t n_p, int neighbor_index, int64_t k_prime) {
	//�޾ƿ� kprime�� ����������� �θ𿡼��� Ű
	//�޾ƿ� �̿� �ε����� ���ε����ƴ� -2 ����
	//n_p�� �� �̿�
	int i, j, neighbor_insertion_index, n_end;
	pagenum_t tmp;

	page_t * t_p = (page_t *)calloc(1, PAGESIZE);
	page_t * node = (page_t *)calloc(1, PAGESIZE);//������ ���
	page_t * neighbor = (page_t *)calloc(1, PAGESIZE);//���ļ� ���� ���
	
	pagenum_t change_neighbor;
	pagenum_t change_die;

	 //�����尡 ���� �����̸� �����ʳ��� ��ģ��
	//��� ������ ��忡 ���̴°� �ƴϰ� ������ �������� ���δ�.
	//�׸��� ������ ��带 �����ϴ� ����
	if (neighbor_index == -2) {

		//nei�ȿ��� ������ 
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
	 n���� Ű �� �����͸� �����ϱ� ���� ���� ����� ������.
	 n�� �̿��� n�� ���� ������ �ڽ��� ��쿡 �ڸ��� �ٲپ��ٴ� ���� ���.
	 */

	neighbor_insertion_index = neighbor->num_key;

	 //������ �ƴѰ��
	if (!node->is_leaf) {

		neighbor->branch[neighbor_insertion_index].key = k_prime;
		neighbor->branch[neighbor_insertion_index].child = node->right_left;
		neighbor->num_key++;
		//ok

		
		
		//�̿����� �Ű��ִ� ����
		for (i = neighbor_insertion_index + 1, j = 0; j < node->num_key; i++, j++) {
			neighbor->branch[i].key = node->branch[j].key;
			neighbor->branch[i].child = node->branch[j].child;
			neighbor->num_key++;
			
		}

		//�������� �� �Űܿ����Ƿ� �θ� ��� �����Ұ���-- �θ������ x

		 //���� n�� �ڽĵ鵵  neighbor�� ����Ű�� ��������
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

	//�����ΰ��
	/*
	 key�� 0�� �̹Ƿ� �ٷ�free ,�׸��� �ùٸ� right page�� ����Ű�� �缳��
	 */

	else {

		if(neighbor_index != -2){

		neighbor->right_left = node->right_left;
		file_free_page(change_die);
		}
		else {//kprime is parent's index 0's key


		//�̿����� �Ű��ִ� ����
		for (i = neighbor_insertion_index, j = 0; j < node->num_key; i++, j++) {



			neighbor->record[i].key = node->record[j].key;
			strcpy(neighbor->record[i].val, node->record[j].val);
			neighbor->num_key++;
			
		}
		neighbor->right_left=node->right_left;
		file_free_page(change_die);
		
		}
		

		
	}

	//n�� ������ ���������ش�

	file_write_page(change_neighbor,neighbor);
	delete_entry(parent, k_prime);

	free(p);
	free(neighbor);
	
	free(node);

	return n;
}

pagenum_t redistribute_nodes(pagenum_t n, pagenum_t n_p, int neighbor_index, int k_prime_index, int64_t k_prime) {

	//delay merge������  node�� ���� leaf�� �� x
	pagenum_t tmp;
	int i;
	page_t * t_p = (page_t *)calloc(1, PAGESIZE);
	page_t * node = (page_t *)calloc(1, PAGESIZE);//������ ���
	page_t * neighbor = (page_t *)calloc(1, PAGESIZE);//���ļ� ���� ���

	file_read_page(n,node);
	file_read_page(n_p,neighbor);
	pagenum_t parent = node->parent;
	page_t * p = (page_t *)calloc(1, PAGESIZE);
	file_read_page(parent, p);


	/* Case: n has a neighbor to the left.
		�̿��� ������ Ű-���� ���� �̿��� ������ ������ n�� ���� ������ ���ʽÿ�.
	�״ϱ� �̰� '�̿� - ��' �̷��� ������ �̿��� �ϳ��� ������ ��ܿ��°���
	�� ��ġ�� ��ġ�ϱ� ��ġ�� ��� �������� ��
	 */

	if (neighbor_index != -2) {

		//������ �ƴѰ�� 
		//����̶� ������ �ƴ϶�°� �ڽĵ� �ִٴ� �Ҹ��ϱ� �ڽĵ鵵 �Ű��ִ� �۾��ʿ� - tmp
		if (!node->is_leaf) {
			//�ϴ� �տ� ���ڸ� ����ߵǴϱ� ��ĭ�� �߹о��ش�
		

			//child�����͵��� �̵�
			node->branch[0].child = node->right_left;
			node->right_left = neighbor->branch[neighbor->num_key - 1].child;
			
			//������ �ڽ��� �θ����� �ٽ�
			tmp = node->right_left;
			file_read_page(tmp, t_p);
			t_p->parent = n;
			file_write_page(tmp, t_p);
			
			//�̿��� Ű�ϳ��� keyprime index�� �θ�� �ø���
			//���ڸ����� �θ��� key prime�� �����´�(�����е� �׸� ����)
			node->branch[0].key = k_prime;
			p->branch[k_prime_index].key = neighbor->branch[neighbor->num_key - 1].key;

			//�̿����� �ڿ� ��ĭ�� �ʱ�ȭ
			neighbor->branch[neighbor->num_key - 1].child = -1;
			neighbor->branch[neighbor->num_key - 1].key = -1;

			
		}

		//�����ΰ�� -  ����� �ڸ��� �Ű��ش�
		//�ڽ��� �����Ƿ� �Ű澵�ʿ� ����
		else {
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
		//�����ΰ�� - �ؿ� �ڽĵ鿡 ���� ���� �ʿ�x
		if (node->is_leaf) {
			//������� ������ �ּ�ũ�⸦ �������� ������ �̿����� ��������
			//���������� capacity�� �ʰ��ϰԵǼ� ���Լ��� �Ҹ��µ� 
			//�̹��� ��� ������ ������ numkey�� 0�̹Ƿ� �ʰ��� �Ұ���
			//�� �� �Լ� �ȿ����� leaf�� ���� ����� ���� �ʾƵ� ��
		}

		//������ �ƴѰ�� �ؿ��ڽĵ鿡 ���� ������ �ʿ�
		else {
			//�θ��ʿ��� keyprime�� ������� �̿��� child pointer�� �����´�
			node->branch[0].key = k_prime;
			node->branch[0].child = neighbor->right_left;
			neighbor->right_left = neighbor->branch[0].child;
		
			tmp = node->branch[0].child;
			file_read_page(tmp, t_p);
			t_p->parent = n;
			p->branch[k_prime_index].key = neighbor->branch[0].key;
			file_write_page(tmp, t_p);


			//�̿��� ���� ���� ��ĭ�� ����������Ƿ� ��ܿ;���
			for (i = 0; i < neighbor->num_key-1; i++) {
				neighbor->branch[i].key = neighbor->branch[i + 1].key;
				neighbor->branch[i].child = neighbor->branch[i + 1].child;
			}
			neighbor->branch[i].key=-1;
			neighbor->branch[i].child=-1;
		}
	}


	/*
	 n�� ���� �� ���� ����� �� ���� �����͸� �� ������ ������,
	 �̿��� ���� �� ���� �� ���� ������ �ִ�.
	 �״ϱ� �ϳ� �����Դ� �̸�
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

	//�޾ƿ� ���������� �� Ű�� ����


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

	//�� ���������� Ű�� ����� 

	remove_entry_from_node(n, key);
	file_read_page(n,node);


	//������ ��Ʈ��忴�� �� 
	if (n == head->root_page) {

		file_write_page(n,node);
		free(node); free(p); free(head);
		
		return adjust_root(n);
	}

	/*
	 ���� �� ������ ����� �ּ� ��� ũ�⸦ �����Ѵ�.�ٵ� �츮�� delay merge�� �ҰŴϱ� �ʿ�x
	 Ű ������ 0 �̻��̶�� �׳� merge���ʿ� x
	 */

	if (node->num_key > 0) {

		file_write_page(n,node);

		free(node); free(p); free(head);
		return n;
	}

	//key������ 0�� �Ȱ��

	//�̿��� �޾ƿ´�
	neighbor_index = get_neighbor_index(n);


	//key������ ����--- �Ƹ� ����
	//-1�̶�°� �����̿��� ��� ������ �̿��� ���ϴ°��
	//���⼭ Ű�������ε����� ����� Ű�� �θ��� �ε���
	//�ǿ����� ���� -1��° Ű�� ���⶧���� 0��° Ű�� �������Ѽ� 
	//�ش� �ε����� child�� right_left�� �� �� �ְ� ���ش�.
	k_prime_index = neighbor_index == -2 ? 0 : neighbor_index + 1;
	//Ű�������� ���� �޾ƿ� �ε����� ���� �θ��ʿ� ����� �� Ű���� ����
	//��� ���� �ǿ������̶�� �ڽ��� Ű�� �����Ƿ� ����ó�� Ű�� 0��°Ű�� ����
	k_prime = p->record[k_prime_index].key;

	//-2�̶�� �ڽ��� ���� ���� �̿��� �����Ƿ� �����ʿ� �ű��
	int real_nei=neighbor_index;
	if (neighbor_index == -2)real_nei = 0;

	page_t * neighbor = (page_t *)calloc(1, PAGESIZE);
	pagenum_t nei_p = p->branch[real_nei].child;
	file_read_page(nei_p, neighbor);

	capacity = node->is_leaf ? leaf_order : node_order;

	//���̿��ε��� �ȳѰ��� -2�� �Ǻ������ϰ� �Ѱ���
	/* Coalescence. */
	//�̰��� ���ƴµ� ���� ok�϶�
	if (neighbor->num_key + node->num_key < capacity) {

		file_write_page(n,node);
		free(neighbor); free(node); free(p); free(head);
		return coalesce_nodes(n, nei_p, neighbor_index, k_prime);
	}

	/* Redistribution. */
	//�ի��µ� ���� �ʰ��� ������
	else
	{

		file_write_page(n,node);
		free(neighbor); free(node); free(p); free(head);
		return redistribute_nodes(n, nei_p, neighbor_index, k_prime_index, k_prime);
	}
}


