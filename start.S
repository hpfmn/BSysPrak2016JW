.section .init

.global _start
_start:
	movw	r0, #:lower16:exception
	movt	r0, #:upper16:exception
	MCR	p15,0,r0,c12,c0,0 @set VBAR

	@cps 	#23	@ switch to ABT mode
	@mov 	r0, #0x5ffffc
	@mov	sp, r0
	@cps	#27	@ switch to UND mode
	@mov 	r0, #0x4ffffc
	@mov	sp, r0
	@cps	#18	@ switch to IRQ mode
	@mov 	r0, #0x6ffffc
	@mov	sp, r0
	@cps	#17	@ switch to FIQ mode
	@mov 	r0, #0x7ffffc
	@mov	sp, r0
	@cps	#31	@ switch to SYS mode
	@mov 	r0, #0x8ffffc
	@mov	sp, r0
	@cps	#19	@ switch to SVC Mode
	@mov	sp, #0x3ffffc
	mov r0, #0x500000
	sub r0,r0,#4
	mov sp, r0

	@ enable IRQs
	bl enableIRQs

        bl      init
.Lend:  
        b       .Lend

