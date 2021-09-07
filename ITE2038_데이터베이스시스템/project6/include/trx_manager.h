#ifndef __TRX_MANAGER_H__
#define __TRX_MANAGER_H__
#include "lock_manager.h"
//#include <windows.h>//Sleep(1000)=1��
typedef struct Trx_m Trx_m;
typedef struct Trx Trx;

//���� ��ũ�� ����Ʈ�� ���� 
typedef struct Trx_m {
	int64_t global_trx_id;//����ũ ���̵� ����
	Trx* trx_table_head;
	Trx* trx_table_tail;

}Trx_m;

typedef struct Trx {
	int trx_id;//�ڽ��� ����ũ ���̵�
	lock_t * trx_lock_head;//�ڽ��� txn�� �����ϴ� lock�� ����
	Trx * next;//����txn 
	Trx* pre;//����txn
	int state;//1=running 2=commiting 3=aborting
	int lastLSN;
}Trx;


Trx_m T_M;

#define NUM_TRX 2
#define HEAD - 2
#define TAIL -3
#define ABORT -4
#define CYCLE -5

//�����ϳ� find_page�� �� �ؿ� �Լ��� �� ���� -> �̰����� ȭ����^^ + acquire���� �պ���
//����ϳ� ������ �������ϰ� �ݿ��ϳ� ����� ���� + q&a ���󺸱�
//����ϵ� ����� �Ͽ��ϵ� ����� �� ��Ű�ۼ�

int trx_init();
int trx_begin();
int trx_commit(int trx_id);
int checkDead(lock_t * front,int back_id);
int trx_abort(int trx_id);
int trx_lock(int trx_id, lock_t * lock);
Trx* trx_find(int trx_id);
int trx_dead_check(lock_t* obj,int back_id);
int trx_cycle(lock_t* obj,int back_id);
int trx_duplicate(lock_t* obj,int back_id);
int trx_error(lock_t* obj);
void printTrx();
void printTrx_lock(lock_t * head);

pthread_mutex_t trx_latch;


#endif
