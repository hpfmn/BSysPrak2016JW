#ifndef MMU_H
#define MMU_H
#define MAX_PIDS 16

typedef unsigned int MMU_TT[4096];
typedef enum mmu_rights mmu_rights;
enum mmu_rights {
	mmu_permission_fault = 0b00 << 10,
	mmu_readwrite_pl1 = 0b01 << 10,
	mmu_readonly = 0b10 << 10,
	mmu_readwrite = 0b11 << 10,
};

void enable_mmu();
void disable_mmu();
void enable_icache();
void enable_ducache();
void disable_icache();
void disable_ducache();
void init_mmu();
void tlbiall();
void set_ttbr0(unsigned int* address);
unsigned int get_ttbr0();
unsigned int get_dacr();
unsigned int get_sctlr();
unsigned int get_dfar();
unsigned int get_dfsr();
unsigned int get_ifar();
unsigned int get_ifsr();
void set_dacr(unsigned int dacr);
void mmap(int pid, int from, int to, int rights);
void minvalidate(int pid, int from);
int getMapping(int pid, int from);
unsigned int *get_mmu(int pid);


#endif
