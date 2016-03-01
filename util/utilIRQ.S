.global enableIRQs

enableIRQs:
	MRS	r0, CPSR
	bic	r0, r0, #0x80
	MSR	CPSR, r0
	mov	pc, lr

.global disableIRQs

disableIRQs:
	MRS	r0, CPSR
	orr	r0, r0, #0x80
	MSR	CPSR, r0
	mov	pc, lr
