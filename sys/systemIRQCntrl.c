#define BADDR	0x3f00b000
#define BIRQ_PEND 0x200 //IRQ basic pending
#define IRQ1_PEND 0x204 //IRQ pending 1
#define IRQ2_PEND 0x208 //IRQ pending 2
#define FIQ_CNTRL 0x20C //FIQ control
#define IRQ1_ENB 0x210 //Enable IRQs 1
#define IRQ2_ENB 0x214 //Enable IRQs 2
#define BIRQ_ENB 0x218 //Enable Basic IRQs
#define IRQ1_DSB 0x21C //Disable IRQs 1
#define IRQ2_DSB 0x220 //Disable IRQs 2
#define BIRQ_DSB 0x224 //Disable Basic IRQs
#include "systemIRQCntrl.h"
#include "../util/memReadWrite.h"

int interruptHappend(irqList intr)
{
	if (intr <=7)
	{
		return (read_u32(BADDR+BIRQ_PEND) & (1 << intr))==(1 << intr);
	}
	else if(intr > 7 && intr <32)
	{
		return (read_u32(BADDR+IRQ1_PEND) & (1 << intr))==(1 << intr);
	}
	intr-=32;
	return (read_u32(BADDR+IRQ2_PEND) & (1 << intr))==(1 << intr);
}

void enableIRQ(irqList intr)
{
	if (intr <= 7)
	{
		write_u32(BADDR+BIRQ_ENB, 1 << intr);	
	}
	else if(intr > 7 && intr <32)
	{
		write_u32(BADDR+IRQ1_ENB, 1 << intr);	
	}
	else if (intr > 31)
	{
		intr-=32;
		write_u32(BADDR+IRQ2_ENB, 1 << intr);	
	}
}

void disableIRQ(irqList intr)
{
	if (intr <= 7)
	{
		write_u32(BADDR+BIRQ_DSB, 1 << intr);	
	}
	else if(intr > 7 && intr <32)
	{
		write_u32(BADDR+IRQ1_DSB, 1 << intr);	
	}
	else if (intr > 31)
	{
		intr-=32;
		write_u32(BADDR+IRQ2_DSB, 1 << intr);	
	}
}
