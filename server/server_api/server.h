#ifndef __SERVER_H__
#define	__SERVER_H__

#include "message.h"

typedef void SERVER;

SERVER *serverInit(const char *map, unsigned int players, int port);
void serverLoop(unsigned int msec);
void serverDestroy();


#endif
