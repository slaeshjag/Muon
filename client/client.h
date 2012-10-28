#ifndef CLIENT_H
#define CLIENT_H

#include "../server/server_api/server.h"

DARNIT_SOCKET *sock;
MESSAGE_RAW msg_recv;
void *msg_recv_offset;
unsigned char msg_recv_payload[512];
unsigned char *msg_recv_payload_offset;
int player_id;

void (*client_message_handler)(MESSAGE_RAW *, unsigned char *);

int client_init(char *host, int port);
int client_check_incomming();

#endif
