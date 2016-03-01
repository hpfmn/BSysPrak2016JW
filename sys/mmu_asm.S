@MCR - move to coprocessor - coproc, opc1, Rt, CRn, CRm(, opc2)
@MRC - move from coprocessor to arm reg - coproc, opc1, Rt, CRn, CRm(, opc2) 

.global enable_mmu
enable_mmu:
	push {r0}
	@SCTLR cp,CRn=c1,opc1=0,CRm=c0,opc2=0
	MRC p15,0,r0,c1,c0,0
	orr r0,r0,#1
	MCR p15,0,r0,c1,c0,0
	pop {r0}
	mov pc,lr

.global disable_mmu
disable_mmu:
	push {r0}
	@SCTLR cp,CRn=c1,opc1=0,CRm=c0,opc2=0
	MRC p15,0,r0,c1,c0,0
	bic r0,r0,#1
	MCR p15,0,r0,c1,c0,0
	pop {r0}
	mov pc,lr

.global enable_icache
enable_icache:
	push {r0}
	@SCTLR cp,CRn=c1,opc1=0,CRm=c0,opc2=0
	MRC p15,0,r0,c1,c0,0
	orr r0,r0,#0x1000 @I bit is bit 12
	MCR p15,0,r0,c1,c0,0
	pop {r0}
	mov pc,lr

.global disable_icache @instruction cache
disable_icache:
	push {r0}
	@SCTLR cp,CRn=c1,opc1=0,CRm=c0,opc2=0
	MRC p15,0,r0,c1,c0,0
	bic r0,r0,#0x1000 @I bit is bit 12
	MCR p15,0,r0,c1,c0,0
	pop {r0}
	mov pc,lr

.global disable_ducache @data and unified cache
disable_ducache:
	push {r0}
	@SCTLR cp,CRn=c1,opc1=0,CRm=c0,opc2=0
	MRC p15,0,r0,c1,c0,0
	bic r0,r0,#0x4 @I bit is bit 2
	MCR p15,0,r0,c1,c0,0
	pop {r0}
	mov pc,lr

.global enable_ducache @data and unified cache
enable_ducache:
	push {r0}
	@SCTLR cp,CRn=c1,opc1=0,CRm=c0,opc2=0
	MRC p15,0,r0,c1,c0,0
	orr r0,r0,#0x4 @I bit is bit 2
	MCR p15,0,r0,c1,c0,0
	pop {r0}
	mov pc,lr

.global set_ttbr0
set_ttbr0:
	@TTBR0 cp15,CRn=c2,opc1=0,CRm=c0,opc2=0 
	MCR p15,0,r0,c2,c0,0
	mov pc,lr

.global get_dacr
get_dacr:
	MRC p15,0,r0,c3,c0,0 @DACR
	mov pc,lr

.global set_dacr
set_dacr:
	MCR p15,0,r0,c3,c0,0 @DACR
	mov pc,lr

.global get_sctlr
get_sctlr:
	MRC p15,0,r0,c1,c0,0
	mov pc,lr
.global get_dfar
get_dfar:
	MRC p15,0,r0,c6,c0,0
	mov pc,lr
.global get_dfsr
get_dfsr:
	MRC p15,0,r0,c5,c0,0
	mov pc,lr
.global get_ifar
get_ifar:
	MRC p15,0,r0,c6,c0,2
	mov pc,lr
.global get_ifsr
get_ifsr:
	MRC p15,0,r0,c5,c0,1
	mov pc,lr

.global tlbiall
tlbiall:
	MCR p15,0,r0,c8,c7,0
	mov pc,lr
