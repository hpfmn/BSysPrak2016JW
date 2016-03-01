ENTRY(_start)
SECTIONS
{
	. = 0x00100000; /* Kernel Code */ 
	.text : { 
		*(.init)
		start.o(.text*)
		sys/sys.o(.text*)
		init.o(.text*)
		sys/exception.o(.text*)
		sys/systemIRQCntrl.o(.text*)
		drv/timer.o(.text*)
		drv/serial_drv.o(.text*)
		sys/except.o(.text*)
		lib/queue.o(.text*)
		sys/threadCntrl.o(.text*)
		util/utilIRQ.o (.text*)
		sys/kprintf.o(.text*)
		sys/mmu.o(.text*)
		sys/mmu_asm.o(.text*) }
	.rodata : {
		start.o(.rodata*)
		sys/sys.o(.rodata*)
		init.o(.rodata*)
		sys/exception.o(.rodata*)
		sys/systemIRQCntrl.o(.rodata*)
		drv/timer.o(.rodata*)
		drv/serial_drv.o(.rodata*)
		sys/except.o(.rodata*)
		lib/queue.o(.rodata*)
		sys/threadCntrl.o(.rodata*)
		util/utilIRQ.o (.rodata*)
		sys/kprintf.o(.rodata*)
		sys/mmu.o(.rodata*)
		sys/mmu_asm.o(.rodata*) }

	. = 0x00200000; /* kernel Data */
	.data : {
		start.o(.data)
		sys/sys.o(.data)
		init.o(.data)
		sys/exception.o(.data)
		sys/systemIRQCntrl.o(.data)
		drv/timer.o(.data)
		drv/serial_drv.o(.data)
		sys/except.o(.data)
		lib/queue.o(.data)
		sys/threadCntrl.o(.data)
		util/utilIRQ.o (.data)
		sys/kprintf.o(.data)
		sys/mmu.o(.data)
		sys/mmu_asm.o(.data) }
	.bss : {
		start.o(.bss)
		sys/sys.o(.bss)
		init.o(.bss)
		sys/exception.o(.bss)
		sys/systemIRQCntrl.o(.bss)
		drv/timer.o(.bss)
		drv/serial_drv.o(.bss)
		sys/except.o(.bss)
		lib/queue.o(.bss)
		sys/threadCntrl.o(.bss)
		util/utilIRQ.o (.bss)
		sys/kprintf.o(.bss)
		sys/mmu.o(.bss)
		sys/mmu_asm.o(.bss) }

	. = 0x00300000; /* User Code+roData */
	utext : { main.o(.text*)
	          sys/sleep.o(.text*)
		  idleThread.o(.text*)
	          sys/syscall.o(.text*)
	          lib/str.o(.text*)
	          lib/triv-printf.o(.text*) }
	urodata : { main.o(.rodata*)
	          sys/sleep.o(.rodata*)
		  idleThread.o(.rodata*)
	          sys/syscall.o(.rodata*)
	          lib/str.o(.rodata*)
	          lib/triv-printf.o(.rodata*) }
}

