#define UART_BASE 0x3F201000
#define UART_DR 0x0
#define UART_FR 0x18

#include "../util/memReadWrite.h"
#include "kprintf.h"
#include <stdarg.h>
#include "../lib/str.h"

int kputChar(char chr)
{
	// busy waiting for TXRDY
	while((read_u32(UART_BASE+UART_FR) & 1<<5));
	write_u32(UART_BASE, (unsigned int) chr);
	// return success
	return 0;
}

char kgetChar(void)
{
	char rtnChr;
	// busy waiting for RXRDY
	while((read_u32(UART_BASE+UART_FR) & 1<<4));
	rtnChr = (char) read_u32(UART_BASE);
	return rtnChr;
}

int kputStr(const char * chr)
{
	int i = 0;
	while(chr[i]!='\0')
	{
		kputChar(chr[i]);
		i++;
	}
	// return success
	return 0;
}

__attribute__((format(printf, 1, 2)))
int kprintf(const char* fmt, ...)
{
	// Declarations
	va_list argPtr; //argumentPointer
	const char* tmpBuffer = fmt;
	va_start(argPtr,fmt);

	// save state, 0 means no % detected,
	// 	       1 means previews char was %
	int state = 0;

	// iterate throught buffer until '\0'
	while(*tmpBuffer != '\0')
	{
		if(state == 0 && *tmpBuffer!='%')
			kputChar(*tmpBuffer);
		if(state == 1)
		{
				switch(*tmpBuffer)
				{
					case 'c':		
						;int c = va_arg(argPtr, int);
						kputChar((char) c);
					break;
					case 's':
						;char* s;
						s = va_arg(argPtr, char*);
						kputStr(s);
					break;
					case 'x':
					{
						;int x = va_arg(argPtr,unsigned int);
						int hLen = hexLength(x,0);
						char sx[hLen+1];
						int2hex(x, hLen, sx, 0);
						kputStr("0x");
						kputStr(sx);
					}
					break;
					case 'd':
					{
						;int x = va_arg(argPtr,unsigned int);
						int hLen = hexLength(x,1);
						char sx[hLen+1];
						int2hex(x, hLen, sx, 1);
						kputStr(sx);
					}
					break;
					case 'p':
					{
						;void* p = va_arg(argPtr,void*);
						int hLen = hexLength((int) p, 0);
						char sp[hLen+1];
						int2hex((int) p, hLen, sp, 0);
						kputStr("0x");
						kputStr(sp);
					}
					break;
					case '%':
						kputChar('%');
					break;
					default:
						kputChar(*tmpBuffer);
					break;
					
				}
				state = 0;
		}
		if(*tmpBuffer == '%' && state == 0)
			state = 1;
		
		tmpBuffer++; //next Char
	}
	va_end(argPtr);
	return 0;
}
