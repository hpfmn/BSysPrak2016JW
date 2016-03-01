#include "mmu.h"
#include "kprintf.h"
#include "../util/globalConst.h"



static MMU_TT MMU_TTs[MAX_PIDS] __attribute__ ((aligned (16384)));

void init_mmu(void)
{
	// For init:
	// bit 31-20: base address
	// sctrl = 1
	// AP2 = 0
	// AP0:1 = 11
	// AFE = 1 
	// domain = 0 
	/*
	ARMv7 Section Eintrag (B3-1326):
	Bit[1:0]     = 0b10     (Section)
	Bit2         = 0     B-Bit Memory region attribute bits, see Memory region attributes on page B3-1366
	Bit3         = 0     C-Bit Memory region attribute bits, see Memory region attributes on page B3-1366
	Bit4         = 0        XN-Bit (Execute Never), see Execute-never restrictions on instruction fetching on page B3-1359
	Bit[8:5]     = 0b0000 (Domain)
	Bit9         = 0        implementation defined
	Bit[11:10]   =         AP-Bits[1:0] Access Permissions bits, see Memory access control on page B3-1356
	Bit[14:12]   = 0b000 TEX-Bits Memory region attribute bits, see Memory region attributes on page B3-1366
	Bit15        = 			AP-Bit[2] (read/write) Access Permissions bits, see Memory access control on page B3-1356
	Bit16        = 0        S-Bit The Shareable bit. Determines whether the addressed region is Shareable memory, see Memoryregion attributes on page B3-1366
	Bit17        = 0        nG-Bit The not global bit. Determines how the translation is marked in the TLB, see Global and process-specific translation table entries on page B3-1378
	Bit18        = 0
	Bit19        = 0        NS-Bit Non-secure bit. If an implementation includes the Security Extensions, for memory accesses from Secure state, this bit specifies whether the translated PA is in the Secure or Non-secure address map, see Control of Secure or Non-secure memory access, Short-descriptor format on page B3-1330
	Bit[31:20]	 =	 		Section base Address
	*/

	int i = 0;
	int j;
	for(j=0;j<MAX_PIDS;j++)
	{
		for (i = 0; i < 4096; ++i)
		{		 
			// kernel code
			if(i==1)
			{
				MMU_TTs[j][i] = i << 20; // base address
				MMU_TTs[j][i] |= mmu_readwrite_pl1; // AP0,AP1
				MMU_TTs[j][i] |= 0b10; // Section
			}
			else if(i==2) // kernel data
			{
				MMU_TTs[j][i] = i << 20; // base address
				MMU_TTs[j][i] |= mmu_readwrite_pl1; // AP0,AP1
				MMU_TTs[j][i] |= 0b10; // Section
			}
			else if(i==3) //user code and rodata
			{
				MMU_TTs[j][i] = i << 20; // base address
				MMU_TTs[j][i] |= mmu_readonly; // AP0,AP1
				MMU_TTs[j][i] |= 0b10; // Section
			}
			else if(i>=4 && i<=8) // PL1 stacks
			{
				MMU_TTs[j][i] = i << 20; // base address
				MMU_TTs[j][i] |= mmu_readwrite_pl1; // AP0,AP1
				MMU_TTs[j][i] |= 0b10; // Section
			}
			else if(i>=0x3f0 && i<0x400) // peripherals
			{
				MMU_TTs[j][i] = i << 20; // base address
				MMU_TTs[j][i] |= mmu_readwrite_pl1; // AP0,AP1
				MMU_TTs[j][i] |= 0b10; // Section
			}
			else
			{
				MMU_TTs[j][i]=0;
			}
		}
	}

	#if DEBUG_MMU
	kprintf("MMU TT Adress %p\r\n", (void *) MMU_TT);
	for(i=0;i<=20;i++)
	{
		kprintf("MMU[%d]=%x\r\n",i,MMU_TT[i]);
	}
	kprintf("DACR=%x\r\n", get_dacr());
	#endif
	
	set_dacr(1);

	#if DEBUG_MMU
	kprintf("DACR=%x\r\n", get_dacr());
	kprintf("SCTLR=%x\r\n", get_sctlr());
	#endif

	set_ttbr0(MMU_TTs[0]);
	enable_mmu();

	#if DEBUG_MMU
	kprintf("SCTLR=%x\r\n", get_sctlr());
	#endif
}

unsigned int* get_mmu(int pid)
{
	return MMU_TTs[pid];
}

void mmap(int pid, int from, int to, int rights)
{
	#if DEBUG_MMAP
	kprintf("mapping for Pid:%d From:%x To:%x\r\n", pid, from, to);
	#endif

	MMU_TTs[pid][from] = to << 20; // base address
	MMU_TTs[pid][from] |= rights; // AP0,AP1
	MMU_TTs[pid][from] |= 0b10; // Section
}

void minvalidate(int pid, int from)
{
	MMU_TTs[pid][from] = 0;
}

int getMapping(int pid, int from)
{
	return MMU_TTs[pid][from]>>20;
}
