#include "lib/triv-printf.h"
#include "sys/syscall.h"
#include "sys/sleep.h"

void print_hello(void)
{
	printf("Welcome to the scheduler test programm\r\n");
	printf("\t! -  means Timer Interrupt happend\r\n");
	printf("\tnewline - means switched to other thread\r\n");
	printf("\twrite any character and it will spawn a new thread thats repeating it during some \"calculation\"\r\n");
}

void idleThread(void *args, int len)
{
//	print_hello();
	args=args;
	len=len;
	while(1)
	{
		sleep();	
	}
}


