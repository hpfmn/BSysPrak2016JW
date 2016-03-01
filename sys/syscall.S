.global putCharSysC
putCharSysC: 
	svc #0
	mov pc,lr

.global getCharSysC
getCharSysC:
	svc #1
	mov pc,lr

.global exitThread
exitThread:
	svc #2
	mov pc,lr

.global createThreadSysC
createThreadSysC:
	svc #3
	mov pc,lr

.global createProcesSysC
createProcesSysC:
	svc #5
	mov pc,lr

.global waitThread
waitThread:
	svc #4
	mov pc,lr
