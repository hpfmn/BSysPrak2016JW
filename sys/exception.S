.balign 32
.global exception

exception:
	.word	0xE7F000F0		/* Reset --> löst Undef aus */
	b undef_trampoline		/* Undefined Instruction */
	b swi_trampoline		/* Software Interrupt */
	b prefabort_trampoline		/* Prefetch Abort */
	b dataabort_trampoline		/* Data Abort */
	.word	0xE7F000F0		/* Reserved --> löst Undef aus*/
	b interrupt_trampoline		/* Interrupt */
	.word	0xE7F000F0		/* Fast Interrupt --> löst Undef aus */

.macro	TRAMPOLINE name lr_offset id handler needs_regs needs_id
  \name:
	/* Rücksprungadresse ggf. korrigieren und sichern */
  .if \lr_offset
  	sub lr, lr, #(\lr_offset)
  .endif
	stmfd sp!, {r0-r12,lr}

	MRS r0,CPSR
	MRS r1,SPSR
	mov r3,sp

	@ get lr and sp from usr mode
	cps #31 @ go into sys
	stmfd r3!, {r13,r14}
	MSR cpsr, r0  @ go back into exception mode

	mov sp,r3
	mov r2,sp
	bl  \handler
	MSR SPSR_fsxc, r0
	MRS r0,CPSR

	mov r3,sp
	cps #31 @ go into sys
	ldmfd r3!, {r13,r14}
	MSR cpsr, r0  @ go back into exception mode
	mov sp,r3

	ldmfd sp!, {r0-r12,pc}^
.endm

/* kein Reset */
TRAMPOLINE undef_trampoline	4 1 _exception_fault	 1 1
TRAMPOLINE swi_trampoline	0 2 _exception_swi	 1 0
TRAMPOLINE prefabort_trampoline	4 3 _exception_pref	 1 1
TRAMPOLINE dataabort_trampoline	8 4 _exception_da	 1 1
/* kein Reserved */
TRAMPOLINE interrupt_trampoline	4 6 _exception_interrupt 0 0
/* kein Fast Interrupt */
