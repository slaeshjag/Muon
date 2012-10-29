#ifndef CLIENT_H
#define CLIENT_H

#include "../server/server_api/server.h"

DARNIT_SOCKET *sock;
MESSAGE_RAW msg_recv;
void *msg_recv_offset;
unsigned char msg_recv_payload[512];
unsigned char *msg_recv_payload_offset;
int player_id;
char *player_names;

void (*client_message_handler)(MESSAGE_RAW *, unsigned char *);

int client_init(char *host, int port);
int client_check_incomming();
void client_message_send(int player_id, int command, int arg_1, int arg_2, char *payload);

#endif
