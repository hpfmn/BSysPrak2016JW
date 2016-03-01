#include "threadCntrl.h"
#include "../drv/timer.h"
#include "../lib/triv-printf.h"
#include "../util/memReadWrite.h"
#include "../util/globalConst.h"
#include "../sys/syscall.h"
#include "../sys/kprintf.h"
#include "../drv/serial_drv.h"
#include "../lib/queue.h"
#include "../sys/mmu.h"

q_elem* nextThread();
int updateThreadRegs(q_elem *next,volatile unsigned int *sp[]);

static queue thread;
static int current_wants_to_be_destroyed=0;
static int force_reschedule=0;
static int force_updateRegs=0;
static int threadsPerPidCount[MAX_PIDS];
static unsigned long free_0_63;

// get next block that is free
// return -1 if there is no free block
int nextFreeBlock()
{
	unsigned long mask;
	int count=0;
	for(mask=1;mask;mask<<=1)
	{
		if(!(mask & free_0_63))
			break;
		count++;
	}
	if(!mask)
		return -1;
	else
	{
		free_0_63|=mask;
		return count;
	}
}

void printThreadRegs()
{
	q_elem* cur = thread.head;
	while(cur!=0)
	{
		int i;
		printf("current %x\r\n", cur);
		printf("thread id %d\r\n", cur->id);
		for(i=0;i<=15;i++)
		{
			printf("r[%d]=%x ", i, cur->tcb.regs[i]);
		}
		printf("cpsr: %x", cur->tcb.cpsr);
		printf("\r\n");
		cur=cur->next;
	}
}

void printListPC(q_elem *list)
{
	q_elem *cur = list;
	while(cur)
	{
		kprintf("%d (%x) -> ", cur->id, cur->tcb.regs[15]);
		cur = cur->next;
	}
}

void printListTime(q_elem *list)
{
	q_elem *cur = list;
	while(cur)
	{
		kprintf("%d (%d) -> ", cur->id, cur->tcb.waitTime);
		cur = cur->next;
	}
}

void printListPrev(q_elem *list)
{
	q_elem *cur = list;
	while(cur)
	{
		kprintf("%d (%d) -> ", cur->id, cur->prev->id);
		cur = cur->next;
	}
}


int doSchedule(int *spsr, volatile unsigned int *sp[], q_elem **next, int timedSchedule)
{
	#if DEBUG_SCHEDULE
	kprintf("do Schedule timed=%d forcere=%d forceup=%d\r\n", timedSchedule, force_reschedule, force_updateRegs);
	#endif
	int returnValue = 0;

	if(!timedSchedule && !force_reschedule)
	{
		(*next) = thread.current;
		return returnValue;
	}

	if(current_wants_to_be_destroyed)
	{
		q_elem *tmp = thread.current;

		thread.thread_count--;
		thread.threads_ready--;
		threadsPerPidCount[tmp->tcb.pid]--;

		// free stack
		unsigned int stackMap = getMapping(tmp->tcb.pid, 0xB+tmp->id)-0xA;
		free_0_63&=(~(1<<stackMap));
		minvalidate(tmp->tcb.pid, 0xB+tmp->id);

		// free data if all threads of a process died
		if(threadsPerPidCount[tmp->tcb.pid]<=0)
		{
			unsigned int dataMap = getMapping(tmp->tcb.pid, 0xA)-0xA;
			free_0_63&=(~(1<<dataMap));
			minvalidate(tmp->tcb.pid, 0xA);
		}

		thread.current = nextThread();

		queueOut(tmp, &thread.head);
		appendElem(tmp, &thread.free_elems);

		force_updateRegs = 1;
		current_wants_to_be_destroyed = 0;
	}
	
	if(!force_updateRegs)
	{
		saveCurrentThread(*sp, *spsr);
		(*next) = nextThread();
	}
	else
	{
		(*next) = thread.current;
	}

	#if DEBUG
	kprintf("waiting: ");
	printListPrev(thread.waiting);
	kprintf("\r\n");
	kprintf("queue: ");
	printListPrev(thread.head);
	kprintf("\r\n");
	kprintf("next %d\r\n", (*next)->id);
	#endif

	if(force_reschedule)
		force_reschedule=0;

	updateTimer(next, timedSchedule);

	#if DEBUG
	kprintf("TIMER NEXT\r\n");
	kprintf("waiting: ");
	printListPrev(thread.waiting);
	kprintf("\r\n");
	kprintf("queue: ");
	printListPrev(thread.head);
	kprintf("\r\n");
	kprintf("next %d\r\n", (*next)->id);
	#endif

	#if DEBUG_THREAD_REGS
	kprintf("next PC %x before THREAD REGS\r\n",(*next)->tcb.regs[15]);
	#endif

	switch(updateThreadRegs((*next),sp))
	{
		// didn't do the context switch, so update LR
		case 1:
			returnValue = 1;
		break;
		case -1:
			kprintf("ERROR no next Thread, something went wrong\r\n");
			while(1);
		break;

	}
	

	return returnValue;
}

void updateTimer(q_elem **next, int timedSchedule)
{
	unsigned int elapsedTime = getTimerLoad() - getTimerTime();
	if(timedSchedule && elapsedTime<getTimerLoad())
		elapsedTime+=getTimerLoad();

	q_elem *curWait = thread.waiting;

	int oldThreads_ready = thread.threads_ready;
	unsigned int shortestWaitLeft = 0xFFFFFFFF;
	while(curWait)
	{
		if(curWait->tcb.status==thread_waiting_time)
		{
			if(curWait->tcb.waitTime <= elapsedTime)
			{
				//kprintf("throwing %d out of waiting", curWait->id);
				curWait->tcb.status=thread_ready;
				q_elem *tmp = curWait->next;
				queueOut(curWait, &thread.waiting);

				thread.threads_ready++;
				insertAtElem(curWait, thread.current);
				#if DEBUG
				kprintf("waiting: ");
				printListPrev(thread.waiting);
				kprintf("\r\n");
				kprintf("queue: ");
				printListPrev(thread.head);
				kprintf("\r\n");
				#endif
				curWait = tmp;
			}
			else
			{
				curWait->tcb.waitTime-=elapsedTime;
				if(shortestWaitLeft > (curWait->tcb.waitTime))
					shortestWaitLeft = curWait->tcb.waitTime;
				curWait=curWait->next;
			}
		}
		else
			curWait=curWait->next;
	}

	if(oldThreads_ready<thread.threads_ready)
	{
		(*next) = thread.current->next;
	}

	unsigned int timerTime;
	if((*next)->priority>0)
	{
		timerTime = (*next)->priority*BASE_TICK;
	}
	else
	{
		timerTime = shortestWaitLeft;
	}
	#if DEBUG
	kprintf("Set timer to %d\r\n", timerTime);
	#endif

	#if DEBUG_TIMER_MAX_TIME
	if(timerTime==0xffffffff)
	{
		kprintf("\r\nwaiting: ");
		printListPC(thread.waiting);
		kprintf("\r\n");
		printListTime(thread.waiting);
		kprintf("\r\nrunning: ");
		printListPC(thread.head);
		kprintf("\r\n");
	}
	#endif

	setTimer(timerTime);
}

void threadIntoWait(int spsr, volatile unsigned int sp[], enum thread_status status)
{
	#if DEBUG
	kprintf("thread into wait\r\n");
	#endif
	// if we're able to put a char into the output Buffer
	// the thread doesn't need to wait
	if(status==thread_waiting_putChr)
	{
		#if DEBUG
		kprintf("try to put char %c\r\n", (char) sp[2]);
		#endif
		if(putChar(sp[2]))
		{
			#if DEBUG
			kprintf("putted char\r\n");
			#endif
			return;
		}
	}
	if(status==thread_waiting_getChr)
	{
		#if DEBUG
		kprintf("try to put char %c\r\n", (char) sp[2]);
		#endif
		char chr = getChar();
		if(chr)
		{
			sp[2] = getChar();
			#if DEBUG
			kprintf("putted char\r\n");
			#endif
			return;
		}
	}


	saveCurrentThread(sp, spsr);
	thread.current->tcb.status = status;

	if(status==thread_waiting_time)
	{
		thread.current->tcb.waitTime = thread.current->tcb.regs[0];
		#if DEBUG
		kprintf("put thread %d into waiting for %dns\r\n",thread.current->id ,thread.current->tcb.waitTime);
		#endif
	}


	q_elem *tmp = thread.current;
	thread.threads_ready--;
	thread.current = nextThread();

	#if DEBUG
	kprintf(" new current %d \r\n", thread.current->id);
	#endif

	queueOut(tmp, &thread.head);
	appendElem(tmp, &thread.waiting);

	force_reschedule = 1;
	force_updateRegs = 1;
}


void threadGotChr()
{
	#if DEBUG
	kprintf("thread got\r\n");
	#endif

	if(thread.waiting==0)
		return;

	q_elem *cur = thread.waiting;
	while(cur != 0 && cur->tcb.status!=thread_waiting_getChr)
	{
		#if DEBUG
		kprintf("%d\r\n", cur->id);
		#endif
		cur = cur->next;
	}
	if(cur!=0 && cur->tcb.status==thread_waiting_getChr)
	{
		#if DEBUG
		kprintf("found waiting for char %d\r\n", cur->id);
		#endif		
		// copy char
		cur->tcb.regs[0] = getChar();
		cur->tcb.status = thread_ready;
		queueOut(cur, &thread.waiting);
		if(insertAtElem(cur, thread.current))
		{
			#if DEBUG
			kprintf("error putting into list\r\n");
			#endif
		}
		//if(thread.threads_ready==1 || thread.current->priority==0)
			force_reschedule = 1;
		thread.threads_ready++;
	}
	#if DEBUG_THREAD_GOTCHR_NOTHREAD
	else
	{
		kprintf("\r\nno thread waiting for char found\r\n");
		kprintf("\r\nwaiting: ");
		printListPC(thread.waiting);
		kprintf("\r\n");
		printListTime(thread.waiting);
		kprintf("\r\nrunning: ");
		printListPC(thread.head);
		kprintf("\r\n");
	}
	#endif
}
void threadRdyChr()
{
	#if DEBUG
	kprintf("thread rdy\r\n");
	#endif
	if(thread.waiting==0)
		return;

	q_elem *cur = thread.waiting;
	while(cur->next != 0 && cur->tcb.status!=thread_waiting_putChr)
		cur = cur->next;
	if(cur!=0 && cur->tcb.status==thread_waiting_putChr)
	{
		if(putChar(read_u32(cur->tcb.regs[0])))
		{
			cur->tcb.status = thread_ready;
			queueOut(cur, &thread.waiting);
			if(insertAtElem(cur, thread.current))
			{
				#if DEBUG
				kprintf("error putting into list\r\n");
				#endif
			}
			if(thread.threads_ready==1 || thread.current->priority==0)
				force_reschedule = 1;
			thread.threads_ready++;
		}
	}
}

q_elem *getCurThread()
{
	return thread.current;
}

void initThread(void (*idleThread)(void* args, int len))
{
	int i;
	for(i=0;i<MAX_PIDS;i++)
	{
		threadsPerPidCount[i]=0;
	}
	thread.head = 0;
	thread.waiting = 0;
	thread.current = 0;
	thread.thread_count = 0;
	thread.threads_ready = 0;
	thread.free_elems = &thread.elements[0];
	thread.elements[0].prev=0;
	thread.elements[0].next=&thread.elements[1];
	thread.elements[0].id=0;
	for (i = 1; i < MAX_THREADS-1; i++)
	{
		thread.elements[i].prev = &thread.elements[i-1];
		thread.elements[i].next = &thread.elements[i+1];
		thread.elements[i].id = i;
	}
	thread.elements[MAX_THREADS-1].prev = &thread.elements[MAX_THREADS-2];
	thread.elements[MAX_THREADS-1].next = 0;
	thread.elements[MAX_THREADS-1].id = MAX_THREADS-1;
	createThread(idleThread,0,0,0,1);
}

// init new thread and append at the beginning of the queue
int createThread(void (*startFun)(void* args, int len), void *args, int len, int prio, int newProcess)
{
	if(thread.free_elems)
	{
		q_elem *newfree = thread.free_elems;
		thread.free_elems = thread.free_elems->next;
		thread.free_elems->prev = 0;
		newfree->next = thread.head;

		// if we're not adding the idle thread and prio is 
		// less than zero throw error
		if(thread.thread_count>0 && prio<=0)
		{
			return -2;
		}
		else
		{
			newfree->priority = prio;
		}

		appendElem(newfree, &thread.head);

		// we're going to copy args to begining of thread stack
		int argbuffer[32]; // if it is a new thread we need to copy data into kernel 
				   // space so we can copy it to the new thread
		if(newProcess)
		{
			len = len>32?32:len; // no buffer overflow ;)
			int i;
			for(i=0;i<len;i++)
			{
				argbuffer[i] = ((int *) args)[i];
			}
			args=argbuffer;
			unsigned int newpid;
			for(newpid=0;threadsPerPidCount[newpid] && newpid<MAX_PIDS;newpid++);
			if(newpid==MAX_PIDS)
				return -1;
			int newMemAdr = nextFreeBlock();
			if(newMemAdr==-1)
				return -1;
			newMemAdr+=0xA;
			mmap(newpid, 0xA, newMemAdr, mmu_readwrite); // data segment - just one per process
			newfree->tcb.pid = newpid;
			set_ttbr0(get_mmu(newpid)); // load new table
			tlbiall(); // flush tlb
		}
		else
		{
			newfree->tcb.pid = thread.current->tcb.pid;
		}

		threadsPerPidCount[newfree->tcb.pid]++;

		int newMemAdr = nextFreeBlock();
		if(newMemAdr==-1)
			return -1;
		newMemAdr+=0xA;
		mmap(newfree->tcb.pid, 0xB+newfree->id, newMemAdr, mmu_readwrite); // stack one for each thread
		newfree->tcb.regs[0] = 0xBffffc + (newfree->id<<20) - (len<<2);

		newfree->tcb.regs[1] = len;
		// SP = USR BASE + Thread Count Offset 64k - args (copied to stack)
		newfree->tcb.regs[13] = newfree->tcb.regs[0]; // sp
		newfree->tcb.regs[14] = (int) &exitThread; // lr 
		//printf("set LR to: %x\r\n", thread.head->tcb.regs[14]);
		newfree->tcb.regs[15] = (int) startFun; // pc
		// init cpsr, user mode enable IRQs, no thumb no FIQ
		newfree->tcb.cpsr = 0b1010000;
		// copy args to stack
		int i;
		int curAddr = newfree->tcb.regs[13];
		for(i=0;i<len;i++)
		{
			int arg = ((int *) args)[i];
			write_u32(curAddr,arg);
			curAddr+=4;
		}
		thread.thread_count++;
		thread.threads_ready++;

		if(newProcess && thread.current)
		{
			set_ttbr0(get_mmu(thread.current->tcb.pid)); // load current table
			tlbiall(); // flush tlb
		}

		// if there was currently just the idle Thread read 
		// and we got a new one -> force reschedule!
		if(thread.threads_ready==2)
		{
		//	setTimer(0);
		        #if DEBUG
		        kprintf("create force \r\n");
		        #endif
			force_reschedule=1;
		}
		return 1;
	}
	else
		return -1;
}
int destroyThread()
{
	current_wants_to_be_destroyed = 1;
	force_reschedule = 1;
	return 1;
}

unsigned int getThreadCounter()
{
	return thread.thread_count;
}
unsigned int getCurrentThreadId()
{
	if (thread.current)
		return thread.current->id;
	else
		return 32;
}

//SP contains: r13,r14 (usr mode), r0-r12,lr
int updateThreadRegs(q_elem *next, volatile unsigned int *sp[])
{
	#if DEBUG_THREAD_REGS
	kprintf("update thread Regs");
	#endif
	if(next!=0)
	{
		if(next==thread.current && !force_updateRegs)
		{
			#if DEBUG_THREAD_REGS
			kprintf("next thread is current one\r\n");
			#endif
			return 0;
		}
		else
		{
			thread.current=next;
			(*sp)[0] = thread.current->tcb.regs[13];
			(*sp)[1] = thread.current->tcb.regs[14];
			(*sp)[15] = thread.current->tcb.regs[15];
			#if DEBUG_THREAD_REGS
			kprintf("next PC should be %x\r\n", thread.current->tcb.regs[15]);
			#endif
			int i;
			for(i=0;i<=12;i++)
			{
				(*sp)[i+2]=thread.current->tcb.regs[i];
			}
			if(force_updateRegs)
				force_updateRegs = 0;
			set_ttbr0(get_mmu(next->tcb.pid));
			tlbiall();
			return 1;
		}
	}
	else
	{
		return -1;
	}
}

//SP contains: r13,r14 (usr mode), r0-r12,lr
void saveCurrentThread(volatile unsigned int sp[], int spsr)
{
	if(thread.current)
	{

		// save thread
		thread.current->tcb.regs[13]=sp[0];
		thread.current->tcb.regs[14]=sp[1];
		thread.current->tcb.regs[15]=sp[15];
		int i;
		for(i=2;i<=14;i++)
		{
			thread.current->tcb.regs[i-2]=sp[i];
		}
		thread.current->tcb.cpsr = spsr;
	}
}

q_elem* nextThread(void)
{
	#if DEBUG
	kprintf("NEXT THREAD UNIT: cur %d", thread.current->id);
	#endif

	if(thread.current && thread.current->next)
	{
		q_elem *next = thread.current->next;
		// if it is the idle thread and there are more
		// threads, skip idle thread
		if(next->priority==0 && thread.threads_ready>1)
		{
			thread.current = next;
			next = nextThread();
		}
		if(next)
			return next;
		else
			return thread.head;
	} else
	{
		if(thread.head->priority==0 && thread.head->next!=0 && thread.threads_ready>1)
			return thread.head->next;
		else
			return thread.head;
	}
}
