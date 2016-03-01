#ifndef KPRINTF_H
#define KRPINTF_H

int kputChar(char chr);
char kgetChar(void);
int kputStr(const char * chr);
__attribute__((format(printf, 1, 2)))
int kprintf(const char* fmt, ...);

#endif
