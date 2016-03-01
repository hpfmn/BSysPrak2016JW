#define UART_BASE 0x3F201000
#define UART_DR 0x0
#define UART_FR 0x18
#define UART_LCRH 0x2c
#define UART_CR 0x30
#define UART_IFLS 0x34
#define UART_IMSC 0x38
#define UART_RIS 0x3c
#define UART_ICR 0x44
#define UART_BUF_SIZE 400

#include "serial_drv.h"
#include "../util/memReadWrite.h"
#include "../util/utilIRQ.h"
#include "../sys/systemIRQCntrl.h"
#include "../sys/threadCntrl.h"
#include "../lib/triv-printf.h"

struct ringBuffer {
	volatile int inPos;
	volatile int outPos;
	char data[UART_BUF_SIZE];
};

static volatile struct ringBuffer UARTinbuf = {.inPos=0, .outPos=0};
static volatile struct ringBuffer UARToutbuf = {.inPos=0, .outPos=0};

static inline int intrHappend(uartintr intr)
{
	return (read_u32(UART_BASE+UART_RIS) & intr) == intr;
}

int addToBuffer(volatile struct ringBuffer *buf, char add_chr)
{
	int returnVal = 0;
	// if buffer is full, throw oldest char away
	if(((buf->inPos+1) % UART_BUF_SIZE) == (buf->outPos))
	{
		buf->outPos = ((buf->outPos)+1) % UART_BUF_SIZE;
		returnVal = -1;
	}
	buf->data[buf->inPos] = add_chr;
	buf->inPos = ((buf->inPos)+1) % UART_BUF_SIZE;
	return returnVal;
}

int getFromBuffer(volatile struct ringBuffer *buf)
{

	if(buf->outPos == buf->inPos)
	{
		return 0;
	}

	int returnVal = (int)  buf->data[buf->outPos];
	buf->outPos = ((buf->outPos)+1) % UART_BUF_SIZE;
	return returnVal;
}

int bufferEmpty(volatile struct ringBuffer *buf)
{
	if(buf->outPos == buf->inPos)
		return 1;
	else
		return 0;
}

int bufferFull(volatile struct ringBuffer *buf)
{
	if(((buf->inPos+1) % UART_BUF_SIZE) == (buf->outPos))
		return 1;
	else
		return 0;
}

int UART_handleIntr(void)
{
	int retVal = 0;
	disableIRQ(uart_int);
	if(intrHappend(UARTRXINTR))
	{
		int readChar = read_u32(UART_BASE);
		addToBuffer(&UARTinbuf, (char) readChar);
		//kprintf("got char %c\r\n", readChar);
		threadGotChr();
		//createThread(&doSomeStuffWithReceivedChar, &readChar, 1, 1);
		retVal = 1;
	}
	if(intrHappend(UARTCTSINTR))
	{
	}
	if(intrHappend(UARTTXINTR))
	{
		if(bufferEmpty(&UARToutbuf))
		{
			UART_disableIntr(UARTTXINTR);
		}
		else
		{
			write_u32(UART_BASE, getFromBuffer(&UARToutbuf) & 0xFF);
			threadRdyChr();
		}
	}
	if(intrHappend(UARTCTSINTR))
	{
	}
	if(intrHappend(UARTRTINTR))
	{
	}
	if(intrHappend(UARTFEINTR))
	{
	}
	if(intrHappend(UARTPEINTR))
	{
	}
	if(intrHappend(UARTBEINTR))
	{
	}
	if(intrHappend(UARTOEINTR))
	{
	}
	write_u32(UART_BASE+UART_ICR, 0);
	enableIRQ(uart_int);
	return retVal;
}

int putChar(char chr)
{
	if(bufferFull(&UARToutbuf))
		return 0;
	addToBuffer(&UARToutbuf, chr);

	// enable TX Interrupts when buffer was empty
	if(!bufferEmpty(&UARToutbuf))
	{
		UART_enableIntr(UARTTXINTR);
	}

	return 1;
}

int getChar(void)
{
	return getFromBuffer(&UARTinbuf);
}

int putStr(const char * chr)
{
	int i = 0;
	while(chr[i]!='\0')
	{
		putChar(chr[i]);
		i++;
	}
	// return success
	return 0;
}

void UART_enableIntr(uartintr intr)
{
	write_u32(UART_BASE+UART_IMSC, read_u32(UART_BASE+UART_IMSC) | intr);
}

void UART_disableIntr(uartintr intr)
{
	write_u32(UART_BASE+UART_IMSC, read_u32(UART_BASE+UART_IMSC) & (~intr));
}

void UART_clearIntr(uartintr intr)
{
	write_u32(UART_BASE+UART_ICR, read_u32(UART_BASE+UART_ICR) & (~intr));
}

void UART_disableFIFO(void)
{
	write_u32(UART_BASE+UART_LCRH, read_u32(UART_BASE+UART_LCRH) & (~(1<<4)));
}
