#ifndef QUEUE_H
#define QUEUE_H
#define MAX_THREADS 32

typedef struct q_elem q_elem;
typedef struct queue queue;

enum thread_status {
	thread_ready = 0,
	thread_waiting_getChr,
	thread_waiting_putChr,
	thread_waiting_time,
};

struct TCB
{
	unsigned int regs[16];
	unsigned int cpsr;
	enum thread_status status;
	unsigned int waitTime;
	unsigned int pid;
};

struct q_elem
{
	volatile struct TCB tcb;
	int id;
	int priority;
	q_elem* next;
	q_elem* prev;
};

struct queue
{
	q_elem* head;
	q_elem* free_elems;
	q_elem* waiting;
	q_elem* current;
	int	thread_count;
	int	threads_ready;
	q_elem elements[MAX_THREADS];
};

int insertElem(q_elem *new, q_elem **list);
int appendElem(q_elem *new, q_elem **list); 
int insertAtElem(q_elem *new,q_elem *at);
int queueOut(q_elem *new, q_elem **list);

#endif
