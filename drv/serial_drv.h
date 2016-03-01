#ifndef SERIAL_H
#define SERIAL_H

enum UARTINTR{UARTCTSINTR =    2, 
	      UARTRXINTR  =   16, 
	      UARTTXINTR  =   32, 
	      UARTRTINTR  =   64, 
	      UARTFEINTR  =  128,
	      UARTPEINTR  =  256, 
	      UARTBEINTR  =  512, 
	      UARTOEINTR  = 1024};
typedef enum UARTINTR uartintr;

void UART_enableIntr(uartintr intr);
void UART_disableIntr(uartintr intr);
void UART_clearIntr(uartintr intr);
int UART_handleIntr(void);
void UART_disableFIFO(void);

int putChar(char chr);
int getChar(void);
int putStr(const char * chr);

#endif
