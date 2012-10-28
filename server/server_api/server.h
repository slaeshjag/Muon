#ifndef __SERVER_H__
#define	__SERVER_H__

#define	API_VERSION			0x10000

#include "message.h"

typedef void SERVER;

SERVER *serverInit(const char *map, unsigned int players, int port);
void serverLoop(unsigned int msec);
void serverDestroy();


#endif
