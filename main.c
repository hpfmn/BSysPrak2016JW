#include "lib/triv-printf.h"
#include "sys/syscall.h"

#define GLOB_COUNT_MAX 16

void main(__attribute__((unused)) void *args, __attribute__((unused)) int len);
void doSomeStuffWithReceivedCharAcitve(void *data, int len);
void doSomeStuffWithReceivedCharPassive(void *data, int len);

void sf(int i)
{
	volatile int j=i*i;
	sf(j);
}

void main(__attribute__((unused)) void *args, __attribute__((unused)) int len)
{
	int buf;
	while(1)
	{
		buf = getCharSysC();
		if (buf >= 'A' && buf <= 'Z')
			createProcesSysC(&doSomeStuffWithReceivedCharAcitve, &buf, 1, 1);
		else
			createProcesSysC(&doSomeStuffWithReceivedCharPassive, &buf, 1, 1);
	}
}

void activeloop(void *data, __attribute__((unused))int len)
{
	int id = *((int *) data);
	volatile int *chr=(volatile int *) 0xA00000;;
	volatile int *globcounter=(volatile int *) 0xA00004;
	int loccounter=0;
	while((*globcounter)<GLOB_COUNT_MAX)
	{
		(*globcounter)++;
		loccounter++;
		printf("%c%d: %x (%d)\r\n",(char) *chr, id, *globcounter, loccounter);
		volatile int j = 0;
		while(j<8)
		{
			volatile int i=0;
			while(i<0xfFFF)
			{
				i++;
			}
			j++;
		}
	}
}

void doSomeStuffWithReceivedCharAcitve(void *data, __attribute__((unused))  int len)
{
	int inp = * ((int *) data);
	int counter = 0;
	*((volatile int *) 0xA00000) = inp;
	*((volatile int *) 0xA00004) = counter;
	int id1=1;
	int id2=2;
	int id3=3;
	createThreadSysC(&activeloop, &id2, 1, 1);
	createThreadSysC(&activeloop, &id3, 1, 1);
	activeloop(&id1,1);
}

void passiveloop(void *data, __attribute__((unused))int len)
{
	int id = *((int *) data);
	volatile int *chr=(volatile int *) 0xA00000;;
	volatile int *globcounter=(volatile int *) 0xA00004;
	int loccounter=0;
	while((*globcounter)<GLOB_COUNT_MAX)
	{
		(*globcounter)++;
		loccounter++;
		printf("%c%d: %x (%d)\r\n",(char) *chr, id, *globcounter, loccounter);
		waitThread(500000);//62500);
	}
}

void doSomeStuffWithReceivedCharPassive(void *data,__attribute__((unused))int len)
{
	int inp = * ((int *) data);
	int counter = 0;
	*((volatile int *) 0xA00000) = inp;
	*((volatile int *) 0xA00004) = counter;
	int id1=1;
	int id2=2;
	int id3=3;
	createThreadSysC(&passiveloop, &id2, 1, 1);
	createThreadSysC(&passiveloop, &id3, 1, 1);
	passiveloop(&id1,1);
}

