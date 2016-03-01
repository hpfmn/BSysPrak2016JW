/* 	Trivial printf-Function
	possible output:
	%c - int argument to unsigned char %s - const char *
	%x - unsigned int to hex output
	%p - void * argument to hex-output
*/

#include <stdarg.h>
#include "triv-printf.h"
#include "str.h"
#include "../sys/syscall.h"

void printf_putStr(char *str)
{
	int i;
	for(i=0;str[i]!='\0';i++)
		putCharSysC(str[i]);
}

__attribute__((format(printf, 1, 2)))
int printf(const char* fmt, ...)
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
			putCharSysC(*tmpBuffer);
		if(state == 1)
		{
				switch(*tmpBuffer)
				{
					case 'c':		
						;int c = va_arg(argPtr, int);
						putCharSysC((char) c);
					break;
					case 's':
						;char* s;
						s = va_arg(argPtr, char*);
						printf_putStr(s);
					break;
					case 'x':
					{
						;int x = va_arg(argPtr,unsigned int);
						int hLen = hexLength(x,0);
						char sx[hLen+1];
						int2hex(x, hLen, sx, 0);
						printf_putStr("0x");
						printf_putStr(sx);
					}
					break;
					case 'd':
					{
						;int x = va_arg(argPtr,unsigned int);
						int hLen = hexLength(x,1);
						char sx[hLen+1];
						int2hex(x, hLen, sx, 1);
						printf_putStr(sx);
					}
					break;
					case 'p':
					{
						;void* p = va_arg(argPtr,void*);
						int hLen = hexLength((int) p, 0);
						char sp[hLen+1];
						int2hex((int) p, hLen, sp, 0);
						printf_putStr("0x");
						printf_putStr(sp);
					}
					break;
					case '%':
						putCharSysC('%');
					break;
					default:
						putCharSysC(*tmpBuffer);
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
