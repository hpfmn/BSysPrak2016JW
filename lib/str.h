#ifndef STR_H
#define STR_H
int slen(char *s);

void int2hex(unsigned int i,int charlen,char *retStr, int dez);
int hexLength(unsigned int i, int dez);

int sCopy(char *s1,int from,int to,char *retStr);
void sInsert(char *s1,char *s2,int pos,char *retStr);
void sConcat(char *s1,char *s2,char *retStr);
#endif
