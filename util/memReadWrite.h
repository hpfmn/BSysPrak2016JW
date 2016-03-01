#ifndef MEMREADWRITE_H
#define MEMREADWRITE_H

static inline int read_u32(unsigned int addr)
{
	return *(volatile unsigned int *)addr;
}

static inline void write_u32(unsigned int addr, unsigned int val)
{
	*(volatile unsigned int *)addr=val;
}

#endif
