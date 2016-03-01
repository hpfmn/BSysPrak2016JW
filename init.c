#include "lib/triv-printf.h"
#include "drv/serial_drv.h"
#include "sys/exception.h"
#include "sys/systemIRQCntrl.h"
#include "sys/threadCntrl.h"
#include "util/memReadWrite.h"
#include "util/globalConst.h"
#include "drv/timer.h"
#include "sys/sleep.h"
#include "sys/syscall.h"
#include "sys/kprintf.h"
#include "sys/mmu.h"
#include "sys/sys.h"
#include "idleThread.h"
#include "main.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

void init(void)
{

	setABTStack(0x6ffffc);
	setUNDStack(0x5ffffc);
	setIRQStack(0x7ffffc);
	setFIQStack(0x8ffffc);
	setSYSStack(0xAffffc);

	UART_disableFIFO();
	//disable caches
	disable_icache();
	disable_ducache();

	setTimer(BASE_TICK);
	timer_ENB();
	enableIRQ(arm_timer);
	enableIRQ(uart_int);
	UART_enableIntr(UARTRXINTR);
	init_mmu();

	#if DEBUG_INIT
	kprintf("init thread\r\n");
	#endif

	initThread(&idleThread);

	#if DEBUG_INIT
	kprintf("init finished\r\n");
	#endif

	createThread(&main,0,0,1,1);

	#if DEBUG_INIT
	kprintf("created main\r\n");
	#endif
//	kprintf("init finished\r\n");
	setTimer(0);
}
