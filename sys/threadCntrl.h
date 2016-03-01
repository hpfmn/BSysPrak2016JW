#ifndef THREAD_CNTRL_H
#define THREAD_CNTRL_H
#include "../lib/queue.h"

typedef void newThread(void *args, int len);

int doSchedule(int *spsr, volatile unsigned int *sp[], q_elem **next, int timedSchedule);

void threadIntoWait(int spsr, volatile unsigned int sp[], enum thread_status status);
void threadGotChr();
void threadRdyChr();

void updateTimer(q_elem **next, int timedSchedule);
void initThread(newThread *idleThread );
void printThreadRegs();
int createThread(newThread *startFun, void *args, int len, int prio, int newProcess);
int destroyThread();
unsigned int getThreadCounter();
unsigned int getCurrentThreadId();
void saveCurrentThread(volatile unsigned int sp[], int spsr);

#endif
