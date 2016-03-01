.global setABTStack
setABTStack:
	cps 	#23	@ switch to ABT mode
	@mov 	r0, #0x5ffffc
	mov	sp, r0
	cps	#19	@ switch to SVC Mode
	mov     pc,lr
.global setUNDStack
setUNDStack:
	cps	#27	@ switch to UND mode
@	mov 	r0, #0x4ffffc
	mov	sp, r0
	cps	#19	@ switch to SVC Mode
	mov pc,lr
.global setIRQStack
setIRQStack:
	cps	#18	@ switch to IRQ mode
	@mov 	r0, #0x6ffffc
	mov	sp, r0
	cps	#19	@ switch to SVC Mode
	mov pc,lr
.global setFIQStack
setFIQStack:
	cps	#17	@ switch to FIQ mode
	@mov 	r0, #0x7ffffc
	mov	sp, r0
	cps	#19	@ switch to SVC Mode
	mov pc,lr
.global setSYSStack
setSYSStack:
	cps	#31	@ switch to SYS mode
	@mov 	r0, #0x8ffffc
	mov	sp, r0
	cps	#19	@ switch to SVC Mode
	mov pc,lr
.global setSVCStack
setSVCStack:
	cps	#19	@ switch to SVC Mode
	@mov	sp, #0x3ffffc
	mov	sp, r0
	mov pc,lr
