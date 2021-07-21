#ifndef __TRX_MANAGER_H__
#define __TRX_MANAGER_H__
#include "lock_manager.h"
//#include <windows.h>//Sleep(1000)=1초
typedef struct Trx_m Trx_m;
typedef struct Trx Trx;

//더블 링크드 리스트로 구현 
typedef struct Trx_m {
	int64_t global_trx_id;//유니크 아이디 제공
	Trx* trx_table_head;
	Trx* trx_table_tail;

}Trx_m;

typedef struct Trx {
	int trx_id;//자신의 유니크 아이디
	lock_t * trx_lock_head;//자신의 txn가 수행하는 lock을 연결
	Trx * next;//다음txn 
	Trx* pre;//이전txn
	int state;//1=running 2=commiting 3=aborting
	int lastLSN;
}Trx;


Trx_m T_M;

#define NUM_TRX 2
#define HEAD - 2
#define TAIL -3
#define ABORT -4
#define CYCLE -5

//수요일날 find_page랑 그 밑에 함수는 다 수정 -> 이게제일 화날듯^^ + acquire좀더 손보기
//목요일날 남은거 마무리하고 금요일날 디버깅 시작 + q&a 영상보기
//토요일도 디버깅 일요일도 디버깅 후 위키작성

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
