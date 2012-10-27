#ifndef __SERVER_H__
#define	__SERVER_H__

typedef void SERVER;

SERVER *serverInit(const char *map, unsigned int players, int port);
void serverLoop(unsigned int msec);
void serverDestroy();


#endif
