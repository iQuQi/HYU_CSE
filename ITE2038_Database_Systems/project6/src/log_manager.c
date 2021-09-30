#include "log_manager.h"
#include "buf_manager.h"

int open_log(char * log_path)
{
	//파일을 새로 생성
	log_fd = open(log_path, O_RDWR | O_CREAT | O_EXCL, 0777);
	if (log_fd > 0)return SUCCESS;
	
	//이미 생성되어있다면 다시 읽어온다.
	log_fd = open(log_path, O_RDWR);
	if (log_fd > 0) return SUCCESS;
	
	return FAIL;
}

int open_msg_log(char *log_msg_path)
{
	//파일을 새로 생성
	int fd = open(log_msg_path, O_RDWR | O_CREAT | O_EXCL, 0777);
	if (fd > 0) return SUCCESS;
	
	//이미 생성되어있다면 다시 읽어온다.
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
	//로그 버퍼의 로그레코드들을 파일로 내려준다.
	off_t off = lseek(log_fd, log->LSN, SEEK_SET);
	ch = write(log_fd, log, BCR_SIZE);



	return SUCCESS;
}

int log_to_file_UP(int trx_id, int log_fd, type_update * log) {
	//로그 버퍼의 로그레코드들을 파일로 내려준다.
	off_t off = lseek(log_fd, log->LSN, SEEK_SET);
	ch = write(log_fd, log,UP_SIZE);

	return SUCCESS;
}

int log_to_file_COM(int trx_id, int log_fd, type_compen * log) {
	//로그 버퍼의 로그레코드들을 파일로 내려준다.
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

	//로그 버퍼에 올릴 자리를 찾는다
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

	//뒤집을 로그를 읽어옴
	//그 log의 old가 new가 되고 new가 old가 되서 strcpy 실행
	//strcpy(log->old_image, old_image);
	//strcpy(log->new_image, new_image);

	//뒤집은 그 로그의 preLSN을 이 로그의 nextundoLSN으로 설정
	//log->next_undo_LSN = ;

	Trx* find = trx_find(trx_id);
	
	log->pre_LSN = find->lastLSN;
	log_M.log_now += COM_SIZE;
	log->LSN = log_M.log_now;
	find->lastLSN = log->LSN;

	//로그 버퍼에 올릴 자리를 찾는다
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

	//로그 버퍼에 올릴 자리를 찾는다
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
	//로그 파일을 연다

	//처음 로그부터 log num까지 쭉 실행

	//만약 log num이 -1이라면 모든 로그(update,compenstate)를 수행

	//만약 consider redo를 해야하는 경우라면 redo skip
	//즉 재실행해야하는 로그의 LSN이 해당 페이지의 lastLSN보다 큰경우만 redo를 해줌
}

void recovery_undo(int log_num)
{
	//루저를 확인한다.

	//로그파일을 열고 가장 뒤쪽 로그부터 시작해서 루저의 로그만 undo실행(log num까지/-1인경우 그냥 normal)

	//write 로그에 대해서는 그냥 그로그를 타겟으로 잡에 compensate log를 발급

	//compensate로그를 만나면 그 로그의 nextundoLSN 값을 읽고 해당하는 로그를 타겟으로 삼고 compensate 로그 생성
	//그 이후부터 그 write log의 pre를 따라가면서 compensate발급

	//(write log)pre가 -1이거나 (compensate)nextundoLSN이 -1이 되면 그 trx에 대한 rollback log발급
}

int analysis()
{
	//로그파일을 연다
	//로그 메세지를 기록
	int log_num=-1;

	while (1) {
		//로그파일에서 하나의 로그 레코드를 읽어온다
		//비긴인경우 체크해둠 

		//abort가 있는데 완전히 다 끝나지 못한경우 loser
		//rollback까지 발견된 경우 winner

		//abort도 없는데 commit 도 없는 경우 loser
		//commit발견시 winner

	}
	//loser리스트를 만든다.
	//위너와 루저의 명단을 메세지로 기록
	
	return log_num;
	
}

int read_log_file(int off)
{
	//먼저 off 주소에서 로그 레코드의 앞 8바이트를 읽어와서 LSN을 읽고
	//endoff까지 읽어온다

	//다음 시작 위치를 리턴해준다
	return SUCCESS;
}
