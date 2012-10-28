#ifndef CLIENT_H
#define CLIENT_H

DARNIT_SOCKET *sock;

int client_init(char *host, int port);

#endif