#include "../drv/timer.h"
#include "../lib/triv-printf.h"
#include "../drv/serial_drv.h"
#include "../sys/systemIRQCntrl.h"
#include "../sys/syscall.h"
#include "../sys/threadCntrl.h"
#include "../util/globalConst.h"
#include "../util/memReadWrite.h"
#include "../sys/kprintf.h"
#include "../sys/mmu.h"
#include "../main.h"

int _exception_interrupt(__attribute__((unused)) int  cpsr, int spsr, volatile unsigned int sp[])
{
	q_elem *next;
	next = 0;
	// arm time highest prority
	if(interruptHappend(arm_timer))
	{
		#if DEBUG_TIMER_IRQ
		kprintf("TIMER IRQ HAPPEND\r\n");
		#endif

		doSchedule(&spsr, &sp, &next,1);
		timerIRQ_CLR();

		#if DEBUG_TIMER_IRQ
		kprintf("next PC:%x PID:%d ID:%d\r\n", next->tcb.regs[15], next->tcb.pid, next->id);
		#endif

		return next->tcb.cpsr;
	}
	if(interruptHappend(uart_int))
	{
		UART_handleIntr();
		doSchedule(&spsr, &sp, &next,0);
		return next->tcb.cpsr;
	}
	return spsr;
}

void dumpRegs(int  cpsr, int spsr, volatile unsigned int sp[])
{
	char *processormodes[16];
	processormodes[0] =  "User";
	processormodes[1] =  "FIQ";
	processormodes[2] = "IRQ";
	processormodes[3] = "Supervisor";
	processormodes[6] = "Monitor";
	processormodes[7] =  "Abort";
	processormodes[10] = "Hyp";
	processormodes[11] =  "Undefined";
	processormodes[15] = "System";

	kprintf("Exception happend - it was a %s-Call\r\n", processormodes[cpsr & 0xF]);
	kprintf("CPSR %x, MODE %s\r\n", cpsr, processormodes[cpsr & 0xF]);
	kprintf("SPSR %x, MODE %s\r\n", spsr, processormodes[spsr & 0xF]);

	kprintf("Register Dump:\r\n");
	int i;
	for(i=0;i<13;i++)
		kprintf("\tr[%d] = %x\r\n", i, sp[i+2]);
	
	kprintf("\tr[%d] = %x\r\n", 13, sp[0]);
	kprintf("\tr[%d] = %x\r\n", 14, sp[1]);
	kprintf("\tr[%d] = %x\r\n", 15, sp[15]);
}
//SP contains: r13,r14 (usr mode), r0-r12,lr
int _exception_fault(int  cpsr, int spsr, volatile unsigned int sp[])
{
	// wait
	dumpRegs(cpsr, spsr, sp);
	while(1);
}

int _exception_swi(__attribute__((unused)) int  cpsr, int spsr, volatile unsigned int sp[])
{
	int swi = read_u32(sp[15]-4);
	//SP contains: r13,r14 (usr mode), r0-r12,lr
	static struct q_elem *next;
	next = 0;
	//kprintf("PC before %x\r\n",sp[15]);
	switch(swi & 0xF)
	{
		case putCharSWI:
			#if DEBUG_SWI
			kprintf("PUTCHAR SWI\r\n");
			#endif
			threadIntoWait(spsr, sp, thread_waiting_putChr);
		break;
		case getCharSWI:
			#if DEBUG_SWI
			kprintf("GETCHAR SWI\r\n");
			#endif
			threadIntoWait(spsr, sp, thread_waiting_getChr);
		break;
		case exitSWI:
			#if DEBUG_SWI
			kprintf("EXITSWI\r\n");
			#endif
			destroyThread();
		break;
		case createThreadSWI:
			#if DEBUG_SWI
			kprintf("CREATTHREAD SWI\r\n");
			#endif
			sp[2] = createThread((newThread *) sp[2], (void *) sp[3], sp[4], sp[5],0);
		break;
		case createProcessSWI:
			#if DEBUG_SWI
			kprintf("CREATE PROCESS SWI\r\n");
			#endif
			sp[2] = createThread((newThread *) sp[2], (void *) sp[3], sp[4], sp[5],1);
		break;
		case waitSWI:
			#if DEBUG_SWI
			kprintf("WAIT SWI\r\n");
			#endif
			threadIntoWait(spsr, sp, thread_waiting_time);
		break;
		default:
			asm(".word 0xE7F000F0");
		break;
	}
	doSchedule(&spsr, &sp, &next, 0);
	return next->tcb.cpsr;
}

void printFSRStatusCode(unsigned int fsr)
{
	switch(fsr&0xf)
	{
		case 0b00001:
			kprintf("alignment fault\r\n");
		break;
		case 0b00100:
			kprintf("fault on instruction cache maintenance\r\n");
		break;
		case 0b01100:
			kprintf("sync. ext. abt. on tt walk first level\r\n");
		break;
		case 0b01110:
			kprintf("sync. ext. abt. on tt walk second level\r\n");
		break;
		case 0b11100:
			kprintf("sync. parity err. on tt walk first level\r\n");
		break;
		case 0b11110:
			kprintf("sync. parity err. on tt walk second level\r\n");
		break;
		case 0b00101:
			kprintf("translation fault first level\r\n");
		break;
		case 0b00111:
			kprintf("translation fault second level\r\n");
		break;
		case 0b00011:
			kprintf("access flag fault first level\r\n");
		break;
		case 0b00110:
			kprintf("access flag fault second level\r\n");
		break;
		case 0b10001:
			kprintf("domain fault first level\r\n");
		break;
		case 0b10011:
			kprintf("domain fault second level\r\n");
		break;
		case 0b01101:
			kprintf("permission fault first level\r\n");
		break;
		case 0b01111:
			kprintf("permission fault second level\r\n");
		break;
		case 0b00010:
			kprintf("debug event\r\n");
		break;
		case 0b01000:
			kprintf("synchronous external abort\r\n");
		break;
		case 0b10000:
			kprintf("TLB conflict abort\r\n");
		break;
		case 0b11001:
			kprintf("synchronous parity error on mem access\r\n");
		break;
		case 0b10110:
			kprintf("asynchronous ext. abt.\r\n");
		break;
		case 0b11000:
			kprintf("asynchronous parity error on mem access\r\n");
		break;
	}
}

int _exception_da(int  cpsr, int spsr, volatile unsigned int sp[])  
{
	unsigned int dfsr = get_dfsr();
	unsigned int dfar = get_dfar();
	kprintf("DATA ABORT\r\n");
	kprintf("DFAR (fault Adress)\t %x\r\n", dfar);
	kprintf("DFSR (status reg)\t %x\r\n", dfsr);
	printFSRStatusCode(dfsr);
	switch(dfar)
	{
		case 0:
			kprintf("NULL Pointer Exception\r\n");
		break;
		case 0x9ffffc:
			kprintf("Probably Stackoverflow\r\n");
		break;
	}

	if(dfsr & (1<<11)) // bit 11 == WnR
	{
		kprintf("it was a write attempt\r\n");
	}
	else
	{
		kprintf("it was a read attempt\r\n");
	}

	dumpRegs(cpsr, spsr, sp);
	while(1);
}

int _exception_pref(int  cpsr, int spsr, volatile unsigned int sp[])  
{
	kprintf("PREFETCH ABORT\r\n");
	unsigned int ifsr = get_ifsr();
	kprintf("IFAR %x\r\n", get_ifar());
	kprintf("IFSR %x\r\n", ifsr);
	dumpRegs(cpsr, spsr, sp);
	printFSRStatusCode(ifsr);
	while(1);
}
