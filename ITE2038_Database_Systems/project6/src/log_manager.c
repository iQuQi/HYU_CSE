#include "log_manager.h"
#include "buf_manager.h"

int open_log(char * log_path)
{
	//������ ���� ����
	log_fd = open(log_path, O_RDWR | O_CREAT | O_EXCL, 0777);
	if (log_fd > 0)return SUCCESS;
	
	//�̹� �����Ǿ��ִٸ� �ٽ� �о�´�.
	log_fd = open(log_path, O_RDWR);
	if (log_fd > 0) return SUCCESS;
	
	return FAIL;
}

int open_msg_log(char *log_msg_path)
{
	//������ ���� ����
	int fd = open(log_msg_path, O_RDWR | O_CREAT | O_EXCL, 0777);
	if (fd > 0) return SUCCESS;
	
	//�̹� �����Ǿ��ִٸ� �ٽ� �о�´�.
	fd = open(log_msg_path, O_RDWR);
	if (fd > 0) return SUCCESS;

	return FAIL;
}

int init_log_buf(int capacity)
{

	log_M.frame_capacity = capacity;
	log_M.use_num = 0;
	log_M.logRecords = (log_buffer_S*)calloc(capacity, sizeof(log_buffer_S));
	
	log_M.log_now=-1;
	log_M.head = -1;
	log_M.tail = -1;

	for (int i = 0; i <capacity; i++)
	{
		log_M.logRecords[i].pre = -1;
		log_M.logRecords[i].next = -1;


	}

	return SUCCESS;
}

int log_to_file_BCR(int trx_id,int log_fd ,type_BCR * log) {
	//�α� ������ �α׷��ڵ���� ���Ϸ� �����ش�.
	off_t off = lseek(log_fd, log->LSN, SEEK_SET);
	ch = write(log_fd, log, BCR_SIZE);



	return SUCCESS;
}

int log_to_file_UP(int trx_id, int log_fd, type_update * log) {
	//�α� ������ �α׷��ڵ���� ���Ϸ� �����ش�.
	off_t off = lseek(log_fd, log->LSN, SEEK_SET);
	ch = write(log_fd, log,UP_SIZE);

	return SUCCESS;
}

int log_to_file_COM(int trx_id, int log_fd, type_compen * log) {
	//�α� ������ �α׷��ڵ���� ���Ϸ� �����ش�.
	off_t off = lseek(log_fd, log->LSN, SEEK_SET);
	ch = write(log_fd, log, COM_SIZE);

	return SUCCESS;
}




int log_BCR(int trx_id, int type)
{
	type_BCR *log=(type_BCR*)malloc(sizeof(struct type_BCR));
	if (!log) return FAIL;

	log->log_size = BCR_SIZE;
	log->trx_id = trx_id;
	log->type = type;
	
	Trx* find = trx_find(trx_id);

	log->pre_LSN = find->lastLSN;
	log_M.log_now += BCR_SIZE;
	log->LSN = log_M.log_now;
	find->lastLSN = log->LSN;

	//�α� ���ۿ� �ø� �ڸ��� ã�´�
	int i;
	for (i = 0; i < log_M.frame_capacity; i++)
	{
		if (log_M.logRecords[i].pre == -1 && log_M.logRecords[i].next == -1) {
			break;
		}
	}
	log_M.logRecords[i].frame_BCR = *log;

	if (log_M.use_num == 0) {
		log_M.logRecords[i].pre = HEAD;
		log_M.logRecords[i].next = TAIL;
		log_M.head = i;
		log_M.tail = i;
		
		
	}
	else {
		log_M.logRecords[i].pre = log_M.tail;
		log_M.logRecords[i].next = TAIL;
		log_M.logRecords[log_M.tail].next = i;
		log_M.tail = i;
	}

	log_M.use_num++;

	return log->LSN;
}

int log_compensate(int trx_id, int table_id, int64_t page_num, int target_log,int record_off)
{
	type_compen *log = (type_compen*)malloc(sizeof(struct type_compen));
	if (!log) return FAIL;
	log->log_size = COM_SIZE;
	log->trx_id = trx_id;
	log->type = COMPENSATE;
	

	log->table_id = table_id;
	log->page_num = page_num;
	log->off = 128+128*(record_off);
	log->data_len = 120;

	//������ �α׸� �о��
	//�� log�� old�� new�� �ǰ� new�� old�� �Ǽ� strcpy ����
	//strcpy(log->old_image, old_image);
	//strcpy(log->new_image, new_image);

	//������ �� �α��� preLSN�� �� �α��� nextundoLSN���� ����
	//log->next_undo_LSN = ;

	Trx* find = trx_find(trx_id);
	
	log->pre_LSN = find->lastLSN;
	log_M.log_now += COM_SIZE;
	log->LSN = log_M.log_now;
	find->lastLSN = log->LSN;

	//�α� ���ۿ� �ø� �ڸ��� ã�´�
	int i;
	for (i = 0; i < log_M.frame_capacity; i++)
	{
		if (log_M.logRecords[i].pre == -1 && log_M.logRecords[i].next == -1) {
			break;
		}
	}

	log_M.logRecords[i].frame_compensate=*log;


	if (log_M.use_num == 0) {
		log_M.logRecords[i].pre = HEAD;
		log_M.logRecords[i].next = TAIL;
		log_M.head = i;
		log_M.tail = i;


	}
	else {
		log_M.logRecords[i].pre = log_M.tail;
		log_M.logRecords[i].next = TAIL;
		log_M.logRecords[log_M.tail].next = i;
		log_M.tail = i;
	}

	log_M.use_num++;
	return log->LSN;
}

int log_update(int trx_id, int table_id, int64_t page_num, char * old_image, char * new_image,int record_off)
{
	type_update *log = (type_update*)malloc(sizeof(struct type_update));
	if (!log) return FAIL;
	log->log_size = UP_SIZE;
	log->trx_id = trx_id;
	log->type = UPDATE;
	

	log->table_id = table_id;
	log->page_num = page_num;
	log->off = 128 + 128 * (record_off);
	log->data_len = 120;
	strcpy(log->old_image, old_image);
	strcpy(log->new_image, new_image);

	Trx* find = trx_find(trx_id);

	log->pre_LSN = find->lastLSN;
	log_M.log_now += UP_SIZE;
	log->LSN = log_M.log_now;
	find->lastLSN = log->LSN;

	//�α� ���ۿ� �ø� �ڸ��� ã�´�
	int i;
	for (i = 0; i < log_M.frame_capacity; i++)
	{
		if (log_M.logRecords[i].pre == -1 && log_M.logRecords[i].next == -1) {
			break;
		}
	}

	log_M.logRecords[i].frame_update = *log;
	if (log_M.use_num == 0) {
		log_M.logRecords[i].pre = HEAD;
		log_M.logRecords[i].next = TAIL;
		log_M.head = i;
		log_M.tail = i;


	}
	else {
		log_M.logRecords[i].pre = log_M.tail;
		log_M.logRecords[i].next = TAIL;
		log_M.logRecords[log_M.tail].next = i;
		log_M.tail = i;
	}

	log_M.use_num++;
	return log->LSN;
}

void recovery_redo(int log_num)
{
	//�α� ������ ����

	//ó�� �α׺��� log num���� �� ����

	//���� log num�� -1�̶�� ��� �α�(update,compenstate)�� ����

	//���� consider redo�� �ؾ��ϴ� ����� redo skip
	//�� ������ؾ��ϴ� �α��� LSN�� �ش� �������� lastLSN���� ū��츸 redo�� ����
}

void recovery_undo(int log_num)
{
	//������ Ȯ���Ѵ�.

	//�α������� ���� ���� ���� �α׺��� �����ؼ� ������ �α׸� undo����(log num����/-1�ΰ�� �׳� normal)

	//write �α׿� ���ؼ��� �׳� �׷α׸� Ÿ������ �⿡ compensate log�� �߱�

	//compensate�α׸� ������ �� �α��� nextundoLSN ���� �а� �ش��ϴ� �α׸� Ÿ������ ��� compensate �α� ����
	//�� ���ĺ��� �� write log�� pre�� ���󰡸鼭 compensate�߱�

	//(write log)pre�� -1�̰ų� (compensate)nextundoLSN�� -1�� �Ǹ� �� trx�� ���� rollback log�߱�
}

int analysis()
{
	//�α������� ����
	//�α� �޼����� ���
	int log_num=-1;

	while (1) {
		//�α����Ͽ��� �ϳ��� �α� ���ڵ带 �о�´�
		//����ΰ�� üũ�ص� 

		//abort�� �ִµ� ������ �� ������ ���Ѱ�� loser
		//rollback���� �߰ߵ� ��� winner

		//abort�� ���µ� commit �� ���� ��� loser
		//commit�߽߰� winner

	}
	//loser����Ʈ�� �����.
	//���ʿ� ������ ����� �޼����� ���
	
	return log_num;
	
}

int read_log_file(int off)
{
	//���� off �ּҿ��� �α� ���ڵ��� �� 8����Ʈ�� �о�ͼ� LSN�� �а�
	//endoff���� �о�´�

	//���� ���� ��ġ�� �������ش�
	return SUCCESS;
}
