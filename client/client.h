#ifndef CLIENT_H
#define CLIENT_H

#include "../server/server_api/server.h"

DARNIT_SOCKET *sock;
MESSAGE_RAW msg_recv;
void *msg_recv_offset;
int player_id;

void (*client_message_handler)(MESSAGE_RAW *);

int client_init(char *host, int port);
void client_check_incomming();

#endif
