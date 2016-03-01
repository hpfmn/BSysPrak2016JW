#define TIMER_CLOCK 250000000
#define BADDR	0x3f00b000
#define	T_LOAD 0x400
#define	T_VAL 0x404
#define T_CTRL 0x408
#define T_IRQ_CLR_ACK 0x40C
#define T_M_IRQ 0x414
#define T_PREDIV 0x41C
#define TIRQ 0x3f00b218

#include "../util/memReadWrite.h"
#include "../lib/triv-printf.h"


void setTimer(unsigned int us)
{
	write_u32(BADDR+T_PREDIV, 250);
	write_u32(BADDR+T_LOAD, us);
}

unsigned int getTimerTime()
{
	return read_u32(BADDR+T_VAL);
}

unsigned int getTimerLoad()
{
	return read_u32(BADDR+T_LOAD);
}

int timerIRQ(void)
{
	return read_u32(BADDR+T_M_IRQ)==1;
}

void timerIRQ_CLR(void)
{
	write_u32(BADDR+T_IRQ_CLR_ACK, 0);
}

void timer_ENB(void)
{
//	write_u32(TIRQ, read_u32(TIRQ) | 1 << 0);
	write_u32(BADDR+T_CTRL, read_u32(BADDR+T_CTRL) | 0xA2);
}
