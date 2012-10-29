#ifndef __SERVER_H__
#define	__SERVER_H__

#define	API_VERSION			0x10000
#define	SERVER_PORT_DEFAULT		56789

#include "message.h"
#include "error.h"

typedef void SERVER;

void serverInit();
SERVER *serverStart(const char *map, unsigned int players, int port);
void serverLoop(unsigned int msec);
void serverStop();
int serverIsRunning();


#endif
