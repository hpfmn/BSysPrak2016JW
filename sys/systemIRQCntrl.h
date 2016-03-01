#ifndef SYSTEMIRQCNTRL_H
#define SYSTEMIRQCNTRL_H

typedef enum {
	arm_timer		= 0,
	arm_mailbox 	= 1,
	arm_doorbell0 	= 2,
	arm_doorbell1	= 3,
	gpu0_halted 	= 4,
	gpu1_halted 	= 5,
	illegal_access1 = 6,
	illegal_access0 = 7,	
	aux_int 		= 29,
	i2c_spi_slv_int = 43,
	pwa0 			= 45,
	pwa1 			= 46,
	smi 			= 48,
	gpio_int0	 	= 49,
	gpio_int1 		= 50,
	gpio_int2 		= 51,
	gpio_int3 		= 52,
	i2c_int 		= 53,
	spi_int 		= 54,
	pcm_int 		= 55,
	uart_int 		= 57
} irqList;

int interruptHappend(irqList);
void enableIRQ(irqList);
void disableIRQ(irqList);

#endif
