#ifndef SYS_H
#define SYS_H
void setABTStack(unsigned int adr);
void setUNDStack(unsigned int adr);
void setIRQStack(unsigned int adr);
void setFIQStack(unsigned int adr);
void setSYSStack(unsigned int adr);
void setSVCStack(unsigned int adr);
#endif
