#ifndef __UTIL_H__
#define	__UTIL_H__

#ifdef _WIN32
	#include <winsock2.h>
#else
	#include <arpa/inet.h>
#endif


typedef union {
	unsigned int		i;
	unsigned char		c[4];
} UTIL_ENDIAN_CONVERT;

unsigned int utilHtonl(unsigned int val);
unsigned int utilNtohl(unsigned int val);
char *utilPathTranslate(const char *path);
int utilStringToIntArray(const char *str, const char *delimiter, int *dest, int max_tokens);
void utilBlockToHostEndian(unsigned int *block, int elements);
unsigned int utilStringSum(const char *str);


#endif
