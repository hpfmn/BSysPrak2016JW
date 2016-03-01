.section .init

.global _start
_start:
	movw	r0, #:lower16:exception
	movt	r0, #:upper16:exception
	MCR	p15,0,r0,c12,c0,0 @set VBAR

	mov r0, #0x500000
	sub r0,r0,#4
	mov sp, r0

	@ enable IRQs
	bl enableIRQs

        bl      init
.Lend:  
        b       .Lend

