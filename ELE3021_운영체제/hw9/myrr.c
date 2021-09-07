#include "sched.h"
static void put_prev_task_myrr(struct rq *rq, struct task_struct *p);
static int select_task_rq_myrr(struct task_struct *p, int cpu, int sd_flag, int flags);
static void set_curr_task_myrr(struct rq *rq);
static void task_tick_myrr(struct rq *rq,struct task_struct *p, int oldprio);
static void switched_to_myrr(struct rq *rq, struct task_struct *p);
void init_myrr_rq(struct myrr_rq *myrr_rq);
static void update_curr_myrr(struct rq *rq);
static void enqueue_task_myrr(struct rq *rq, struct task_struct *p, int flags);
static void dequeue_task_myrr(struct rq *rq, struct task_struct *p, int flags);
static void check_preempt_curr_myrr(struct rq *rq, struct task_struct *p,int flags);
struct task_struct *pick_next_task_myrr(struct rq *rq, struct task_struct *prev);
static void prio_changed_myrr(struct rq *rq, struct task_struct *p, int oldprio);

#define MYRR_TIME_SLICE 4
const struct sched_class myrr_sched_class={
	.next=&fair_sched_class,
	.enqueue_task=&enqueue_task_myrr,
	.dequeue_task=dequeue_task_myrr,
	.check_preempt_curr=check_preempt_curr_myrr,
	.pick_next_task=pick_next_task_myrr,
	.put_prev_task=put_prev_task_myrr,
#ifdef CONFIG_SMP
	.select_task_rq=select_task_rq_myrr,
#endif
	.set_curr_task=set_curr_task_myrr,
	.task_tick=task_tick_myrr,
	.prio_changed=prio_changed_myrr,
	.switched_to=switched_to_myrr,
	.update_curr=update_curr_myrr,
};


void init_myrr_rq (struct myrr_rq *myrr_rq)
{
	printk(KERN_INFO "***[MYRR] Mysched class is online \n");
	myrr_rq->nr_running=0;
	INIT_LIST_HEAD(&myrr_rq->queue);

}
static void update_curr_myrr(struct rq *rq){
	

	struct task_struct *curr = rq->curr;
	struct myrr_rq *myrr_rq = &rq->myrr;
	struct sched_myrr_entity *entity = &curr->myrr;
	entity->update_num++;
	printk("***[MYRR] update_curr_myrr	pid=%d update_num=%d\n",curr->pid ,entity->update_num);
	if(entity->update_num > MYRR_TIME_SLICE){
		entity->update_num = 0;
		list_del_init(&curr->myrr.run_list);
		list_add_tail(&entity->run_list, &myrr_rq->queue);
		resched_curr(rq);
	}

}

static void enqueue_task_myrr(struct rq *rq, struct task_struct *p, int flags) {


	struct myrr_rq *myrr_rq = &rq->myrr;
	struct sched_myrr_entity *task = &p->myrr;
	
	list_add_tail(&task->run_list, &myrr_rq->queue);

	rq->myrr.nr_running++;
	printk(KERN_INFO"***[MYRR] enqueue: success cpu=%d, nr_running=%d, pid=%d\n",cpu_of(rq), rq->myrr.nr_running,p->pid);
}
static void dequeue_task_myrr(struct rq *rq, struct task_struct *p, int flags) 
{
	if(rq->myrr.nr_running>0)
	{
		

		list_del_init(&p->myrr.run_list);
		rq->myrr.nr_running--;
		printk(KERN_INFO"\t***[MYRR] dequeue: success cpu=%d, nr_running=%d, pid=%d\n",cpu_of(rq), rq->myrr.nr_running,p->pid);
	}
	else{
	}
	
}
void check_preempt_curr_myrr(struct rq *rq, struct task_struct *p, int flags) {
	printk("***[MYRR] check_preempt_curr_myrr\n");
}
struct task_struct *pick_next_task_myrr(struct rq *rq, struct task_struct *prev)
{
	struct task_struct *next_p = NULL;
	struct sched_myrr_entity *next_se = NULL;
	struct myrr_rq *myrr_rq = &rq->myrr;	

	if(rq->myrr.nr_running==0 ){
		return NULL;
	}
	

	next_se = container_of(myrr_rq->queue.next, struct sched_myrr_entity, run_list);
	next_p = container_of(next_se, struct task_struct, myrr);
	
	printk(KERN_INFO "\t***[MYRR] pick_next_task: cpu=%d, prev->pid=%d,next_p->pid=%d,nr_running=%d\n",cpu_of(rq),prev->pid,next_p->pid,rq->myrr.nr_running);
	return next_p;
}
void put_prev_task_myrr(struct rq *rq, struct task_struct *p) {
	printk(KERN_INFO "\t***[MYRR] put_prev_task: do_nothing, p->pid=%d\n",p->pid);
}
int select_task_rq_myrr(struct task_struct *p, int cpu, int sd_flag, int flags){return task_cpu(p);}
void set_curr_task_myrr(struct rq *rq){
	printk(KERN_INFO"***[MYRR] set_curr_task_myrr\n");
}
void task_tick_myrr(struct rq *rq, struct task_struct *p, int queued) {
	update_curr_myrr(rq);
}
void prio_changed_myrr(struct rq *rq, struct task_struct *p, int oldprio) { }
void switched_to_myrr(struct rq *rq, struct task_struct *p)
{
	resched_curr(rq);
}

