#include "sched.h"
void init_mysched_rq(struct mysched_rq *mysched_rq)
{
	printk(KERN_INFO "***[MYSCHED] Myschedclass is online \n");
	mysched_rq->nr_running = 0;
	INIT_LIST_HEAD(&mysched_rq->queue);
}

static void update_curr_mysched(struct rq *rq)
{
}
static void enqueue_task_mysched(struct rq *rq, struct task_struct *p, int flags) 
{
	rq->mysched.nr_running +=1;
	list_add_tail(&p->mysched.run_list,&rq->mysched.queue);
	printk(KERN_INFO "***[MYSCHED] enqueue: success cpu=%d, nr_running=%d, pid=%d\n", cpu_of(rq), rq->mysched.nr_running, p->pid);
}

static void dequeue_task_mysched(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_INFO "***[MYSCHED] dequeue: start");
	
	if((int)rq->mysched.nr_running >0){
		rq->mysched.nr_running -=1;
		list_del_init(&p->mysched.run_list);
		printk(KERN_INFO "***[MYSCHED] dequeue: success cpu-%d,nr_running=%d, pid=%d\n",cpu_of(rq), rq->mysched.nr_running, p->pid);
	}	
	printk(KERN_INFO "***[MYSCHED] dequeue: end");
}
static void check_preempt_curr_mysched(struct rq *rq, struct task_struct *p, int flags) 
{ 
}
struct task_struct *pick_next_task_mysched(struct rq *rq, struct task_struct *prev)
{	
	struct list_head head = rq->mysched.queue;
	struct list_head *pick_list = head.next;
	struct sched_mysched_entity *parent;
	struct task_struct *next_task;
	if((int)rq->mysched.nr_running >0){
		put_prev_task(rq,prev);
		parent = list_entry(pick_list,struct sched_mysched_entity, run_list);
		next_task = container_of(parent,struct task_struct, mysched);
		printk(KERN_INFO "***[MYSCHED] pick_next_task: success cpu-%d,nr_running=%d, prev->pid=%d, next->pid=%d\n",cpu_of(rq),rq->mysched.nr_running,prev->pid,next_task->pid);
		
		return next_task;
	}
	else {
	
		return NULL;	
	}
}
static void put_prev_task_mysched(struct rq *rq, struct task_struct *p) 
{
	 printk(KERN_INFO "***[MYSCHED] put prev task: do nothing, p->pid=%d\n",p->pid);
}
static int select_task_rq_mysched(struct task_struct *p, int cpu, int sd_flag, int flags)
{
	return task_cpu(p);
}
static void set_curr_task_mysched(struct rq *rq) 
{ 
}
static void task_tick_mysched(struct rq *rq, struct task_struct *p, int queued) 
{ 
}
static void prio_changed_mysched(struct rq *rq, struct task_struct *p, int oldprio) 
{ 
}
/* This routine is called when a task migrates between classes */
static void switched_to_mysched(struct rq *rq, struct task_struct *p)
{
	resched_curr(rq);
}
const struct sched_class mysched_sched_class = {
	.next			= &idle_sched_class,
	.enqueue_task		= enqueue_task_mysched,
	.dequeue_task		= dequeue_task_mysched,
	.check_preempt_curr	= check_preempt_curr_mysched,
	.pick_next_task		= pick_next_task_mysched,
	.put_prev_task		= put_prev_task_mysched,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_mysched,
#endif
	.set_curr_task		= set_curr_task_mysched,
	.task_tick		= task_tick_mysched,
	.prio_changed		= prio_changed_mysched,
	.switched_to		= switched_to_mysched,
	.update_curr		= update_curr_mysched,
};
