#include "sched.h"
static void put_prev_task_mypriority(struct rq *rq, struct task_struct *p);
static int select_task_rq_mypriority(struct task_struct *p, int cpu, int sd_flag, int flags);
static void set_curr_task_mypriority(struct rq *rq);
static void task_tick_mypriority(struct rq *rq, struct task_struct *p, int oldprio);
static void switched_to_mypriority(struct rq *rq, struct task_struct *p);
void init_mypriority_rq(struct mypriority_rq *mypriority_rq);
static void update_curr_mypriority(struct rq *rq);
static void enqueue_task_mypriority(struct rq *rq, struct task_struct *p, int flags);
static void dequeue_task_mypriority(struct rq *rq, struct task_struct *p, int flags);
static void check_preempt_curr_mypriority(struct rq *rq, struct task_struct *p, int flags);
struct task_struct *pick_next_task_mypriority(struct rq *rq, struct task_struct *prev);
static void prio_changed_mypriority(struct rq *rq, struct task_struct *p, int oldprio);
static void queueing_mypriority(struct sched_mypriority_entity *entity, struct mypriority_rq *mypriority_rq, pid_t pid);

#define MYRR_TIME_SLICE 4
#define PRIORITY_NUM 50
const struct sched_class mypriority_sched_class = {
	.next = &fair_sched_class,
	.enqueue_task = &enqueue_task_mypriority,
	.dequeue_task = dequeue_task_mypriority,
	.check_preempt_curr = check_preempt_curr_mypriority,
	.pick_next_task = pick_next_task_mypriority,
	.put_prev_task = put_prev_task_mypriority,
#ifdef CONFIG_SMP
	.select_task_rq = select_task_rq_mypriority,
#endif
	.set_curr_task = set_curr_task_mypriority,
	.task_tick = task_tick_mypriority,
	.prio_changed = prio_changed_mypriority,
	.switched_to = switched_to_mypriority,
	.update_curr = update_curr_mypriority,
};


void init_mypriority_rq(struct mypriority_rq *mypriority_rq)
{
	printk(KERN_INFO "***[MYPRIORITY] Mysched class is online \n");
	mypriority_rq->nr_running = 0;
	mypriority_rq->next_priority = 0;
	INIT_LIST_HEAD(&mypriority_rq->queue);

}

static void update_curr_mypriority(struct rq *rq) {

	struct task_struct *curr = rq->curr;
	struct mypriority_rq *mypriority_rq = &rq->mypriority;
	struct sched_mypriority_entity *entity = &curr->mypriority;
	//�켱���� ���� ���δ�.
	entity->priority++;
	printk("***[MYPRIORITY] aging_mypriority AGING ++	pid=%d priority=%d\n", curr->pid, entity->priority);
	//���� �ι�°�� ���� �켱�������� �ڽ��� �켱 ������ ���ٸ�(���ڰ� �� ũ�ٸ�) ť�� ���ġ ��Ų��.
	if (mypriority_rq->nr_running > 1 && (entity->priority > mypriority_rq->next_priority)) {
		printk("***[MYPRIORITY] AGING -- > REPLACE BEFORE pid=%d priority=%d\n", curr->pid, entity->priority);
		//ť������ ���ġ �۾� - �켱 ����
		list_del_init(&curr->mypriority.run_list);
		mypriority_rq->nr_running--;
		//ť������ ���ġ �۾� - ������ �°� �ٽ� ����
		queueing_mypriority(entity, mypriority_rq, curr->pid);
		mypriority_rq->nr_running++;
		//�ٽ� �����ٸ��� �ǽ�
		resched_curr(rq);
		printk("***[MYPRIORITY] AGING -- > REPLACE AFTER pid=%d\n", curr->pid);
	}

}

static void queueing_mypriority(struct sched_mypriority_entity *entity, struct mypriority_rq *mypriority_rq, pid_t pid) {
	struct list_head *head = mypriority_rq->queue.next;
	struct list_head *now_list = mypriority_rq->queue.next;

	struct task_struct *next_p = NULL;
	struct sched_mypriority_entity *next_se = NULL;
	struct sched_mypriority_entity * now_entity;
	printk("***[MYPRIORITY] queuing mypriority - START pid=%d\n", pid);
	//ť���� �� ������ ã�´�
	while (now_list->next != head) {
		now_entity = container_of(now_list, struct sched_mypriority_entity, run_list);
		if (now_entity->priority > entity->priority) {
			//�� �ڸ��� ã�Ҵٸ� ��ũ�� ����Ʈ�� �����Ѵ�.
			entity->run_list.next = now_list;
			entity->run_list.prev = now_list->prev;
			now_list->prev->next = &entity->run_list;
			now_list->prev = &entity->run_list;

			next_se = container_of(now_list, struct sched_mypriority_entity, run_list);
			next_p = container_of(next_se, struct task_struct, mypriority);
			printk("***[MYPRIORITY] queuing mypriority - LOCATE!!!! pid=%d my next=%d priority=%d\n", pid, next_p->pid, entity->priority);
			break;
		}
		now_list = now_list->next;
	}
	//tail�� ��ġ�ؾ��ϴ� ��� list_add_tail �Լ��� Ȱ��
	if (now_list->next == head) {
		printk("***[MYPRIORITY] queuing mypriority - LOCATE TAIL pid=%d priority=%d\n", pid, entity->priority);
		list_add_tail(&entity->run_list, &mypriority_rq->queue);
	}

	//���� ���μ����� �켱���� ���� �����Ͽ� �д�.
	if (mypriority_rq->nr_running > 1) {
		struct sched_mypriority_entity *find_entity;
		struct list_head *head = mypriority_rq->queue.next;
		find_entity = container_of(head->next, struct sched_mypriority_entity, run_list);
		mypriority_rq->next_priority = find_entity->priority;
		printk("***[MYPRIORITY] queuing mypriority - NEXT PRIORITY VALUE mypriority_rq->next_priority=%d\n", mypriority_rq->next_priority);

	}
}

static void enqueue_task_mypriority(struct rq *rq, struct task_struct *p, int flags) {

	struct mypriority_rq *mypriority_rq = &rq->mypriority;
	struct sched_mypriority_entity *task = &p->mypriority;
	struct task_struct *curr = rq->curr;

	//pid�� �̿��ؼ� task�� �켱������ �ο��Ѵ�.
	task->priority = p->pid % PRIORITY_NUM;
	//�켱������ �°� ť�� ����ִ´�.
	rq->mypriority.nr_running++;
	queueing_mypriority(task, mypriority_rq, p->pid);
	printk(KERN_INFO"***[MYPRIORITY] Enqueue: success cpu=%d, nr_running=%d, pid=%d\n", cpu_of(rq), rq->mypriority.nr_running, p->pid);
	

	//���� ���� �켱������� �����Ͽ� CPU�� ȹ���ϰ� �ȴ�.
	if (mypriority_rq->nr_running > 1 && (&task->run_list == mypriority_rq->queue.next)) {
		//�����ٸ��� ������Ѵ�.
		resched_curr(rq);
		printk(KERN_INFO"***[MYPRIORITY] enqueue - PREEMPT: success cpu=%d, nr_running=%d, now_pid=%d\n", cpu_of(rq), rq->mypriority.nr_running, curr->pid);

	}

}
static void dequeue_task_mypriority(struct rq *rq, struct task_struct *p, int flags)
{
	struct mypriority_rq *mypriority_rq = &rq->mypriority;
	if (rq->mypriority.nr_running > 0)
	{

		//�ϳ� �̻��� ��Ұ� ť�� ������ ��� 
		//�Ѱܹ��� task�� ť���� �����Ѵ�.
		list_del_init(&p->mypriority.run_list);
		//ť ������ ���μ��� ���� ������ ���δ�.
		rq->mypriority.nr_running--;
		printk(KERN_INFO"***[MYPRIORITY] Dequeue: success cpu=%d, nr_running=%d, pid=%d\n", cpu_of(rq), rq->mypriority.nr_running, p->pid);
		if (mypriority_rq->nr_running > 1) {
			//���� 2���̻��� ��Ұ� ������ ��� �ι�°�� ���� �켱���� ���� �缳�� �Ѵ�.
			struct sched_mypriority_entity *find_entity;
			struct list_head *head = mypriority_rq->queue.next;
			find_entity = container_of(head->next, struct sched_mypriority_entity, run_list);
			mypriority_rq->next_priority = find_entity->priority;
			printk("***[MYPRIORITY]  Dequeue - NEXT PRIORITY VALUE mypriority_rq->next_priority=%d\n", mypriority_rq->next_priority);
		}
	}
	else {
	}

}
void check_preempt_curr_mypriority(struct rq *rq, struct task_struct *p, int flags) {
	printk("***[MYPRIORITY] check_preempt_curr_mypriority\n");
}
struct task_struct *pick_next_task_mypriority(struct rq *rq, struct task_struct *prev)
{
	struct task_struct *next_p = NULL;
	struct sched_mypriority_entity *next_se = NULL;
	struct mypriority_rq *mypriority_rq = &rq->mypriority;

	//���� ���� ������� ���μ����� ���ٸ� �ٷ� ����
	if (rq->mypriority.nr_running == 0) {
		return NULL;
	}

	//���� ���� ť�� �ϳ� �̻��� ��� ���μ����� �����Ѵٸ�
	//�������� ����� ���μ���(ť���� ���� ��)�� ��� �Ѱ��ְ� �ȴ�.
	next_se = container_of(mypriority_rq->queue.next, struct sched_mypriority_entity, run_list);
	next_p = container_of(next_se, struct task_struct, mypriority);

	printk(KERN_INFO "***[MYPRIORITY] pick_next_task: cpu=%d, prev->pid=%d,next_p->pid=%d,nr_running=%d\n", cpu_of(rq), prev->pid, next_p->pid, rq->mypriority.nr_running);
	return next_p;
}
void put_prev_task_mypriority(struct rq *rq, struct task_struct *p) {
	printk(KERN_INFO "***[MYPRIORITY] put_prev_task: do_nothing, p->pid=%d\n", p->pid);
}
int select_task_rq_mypriority(struct task_struct *p, int cpu, int sd_flag, int flags) { return task_cpu(p); }
void set_curr_task_mypriority(struct rq *rq) {
	printk(KERN_INFO"***[MYPRIORITY] set_curr_task_mypriority\n");
}
void task_tick_mypriority(struct rq *rq, struct task_struct *p, int queued) {
	//���� �ð����� update �Լ��� ȣ���ϰ� �ȴ�.
	update_curr_mypriority(rq);

}
void prio_changed_mypriority(struct rq *rq, struct task_struct *p, int oldprio) { }
void switched_to_mypriority(struct rq *rq, struct task_struct *p)
{
	resched_curr(rq);
}
