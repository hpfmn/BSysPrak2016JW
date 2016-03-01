#ifndef TIMER_H
#define TIMER_H
void setTimer(unsigned int us);
int timerIRQ(void);
void timerIRQ_CLR(void);
void timer_ENB(void);
unsigned int getTimerTime();
unsigned int getTimerLoad();
#endif
