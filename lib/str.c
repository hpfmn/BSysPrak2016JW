#include "str.h"

int hexLength(unsigned int i, int dez)
{
	int charlen = 0;
	unsigned int curi = i;
	while(curi>0)
	{
		if(dez==1)
			curi=curi / 10;
		else
			curi=curi / 16;
		charlen++;
	}
	if (charlen==0)
		charlen=1;
	return charlen;
}

void int2hex(unsigned int i, int charlen, char * retStr, int dez)
{
	char charSymbols[] = "0123456789ABCDEF";

	retStr[charlen] = '\0';

	unsigned int curi = i;
	int j;
	for(j=charlen-1; j>=0 ;j--)
	{
		if(dez==1)
		{
			retStr[j] = charSymbols[curi % 10];
			curi=curi / 10;
		}
		else
		{
			retStr[j] = charSymbols[curi % 16];
			curi = curi / 16;
		}
	}
	return;
}

int slen(char *s)
{
	int i=0;
	while(s[i]!='\0')
		i++;
	return i;
}

void sConcat(char *s1, char *s2, char * retStr)
{
	int slen1 = slen(s1);
	int slen2 = slen(s2);

	int i;
	for(i=0;i<slen1+slen2;i++)
	{
		if(i<slen1)
		{
			retStr[i]=s1[i];
		}
		if(i>=slen1 && i<slen1+slen2)
		{
			retStr[i]=s2[i-slen1];
		}
	}
	retStr[slen1+slen2]='\0';
	return;
}

void sInsert(char *s1, char *s2, int pos, char * retStr)
{
	int slen1 = slen(s1);
	int slen2 = slen(s2);

	int i;
	for(i=0;i<slen1+slen2;i++)
	{
		if(i<pos)
		{
			retStr[i]=s1[i];
		}
		if(i>=pos && i<pos+slen2)
		{
			retStr[i]=s2[i-pos];
		}
		if(i>=pos+slen2)
		{
			retStr[i]=s1[i-slen2];
		}
	}
	retStr[slen1+slen2]='\0';
	return;
}

int sCopy(char *s1, int from, int to, char * retStr)
{
	int slen1 = slen(s1);
	if(from>=slen1 || to >=slen1)
		return -1;
	if(from>to)
		return -2;

	int i;
	for(i=0;i<to-from;i++)
	{
		retStr[i] = s1[i+from];
	}
	retStr[to-from]='\0';
	return 0;
}
