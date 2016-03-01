#ifndef SYSCALL_H
#define SYSCALL_H

enum SWIs {
	putCharSWI=0,
	getCharSWI,
	exitSWI,
	createThreadSWI,
	waitSWI,
	createProcessSWI
};

void putCharSysC(int buf);
char getCharSysC();
void exitThread();
int  createThreadSysC(void(*startFun)(void* args, int len), void *args, int len, int prio);
int  createProcesSysC(void(*startFun)(void* args, int len), void *args, int len, int prio);
void  waitThread(int ns);

#endif
