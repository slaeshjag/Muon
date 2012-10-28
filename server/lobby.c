#include "server.h"


void lobbyMapSend(unsigned int player) {
	MESSAGE msg;

	msg.player_ID = player;
	msg.command = MSG_SEND_MAP_BEGIN;
	msg.arg[0] = server->map_c.data_len;
	msg.arg[1] = strlen(server->map_c.path);
	msg.extra = (void *) server->map_c.path;
	messageBufferPush(server->player[player].msg_buf, &msg);
	
	msg.player_ID = player;
	msg.command = MSG_SEND_MAP_CHUNK;
	msg.arg[0] = 0;
	msg.arg[1] = server->map_c.data_len;
	msg.extra = server->map_c.data;
	messageBufferPush(server->player[player].msg_buf, &msg);

	msg.player_ID = player;
	msg.command = MSG_SEND_MAP_END;
	msg.arg[0] = 0;
	msg.arg[1] = 0;
	msg.extra = NULL;
	messageBufferPush(server->player[player].msg_buf, &msg);

	return;
}



int lobbyPoll() {
	SERVER_SOCKET *socket;
	int slot;

	if ((socket = networkAccept(server->accept)) == NULL)
		return 0;
	if ((slot = playerSlot(server)) == -1) {
		messageSend(socket, 0, MSG_SEND_SERVER_FULL, 0, 0, NULL);
		networkSocketDisconnect(socket);
		return 0;
	}

	fprintf(stderr, "Accepted a connection\n");
	server->player[slot].status = PLAYER_WAITING_FOR_IDENTIFY;
	server->player[slot].socket = socket;
	server->player[slot].id_req_send = time(NULL);
	*server->player[slot].name = 0;

	server->player[slot].process_recv = PLAYER_PROCESS_NOTHING;
	server->player[slot].process_send = PLAYER_PROCESS_NOTHING;
	server->player[slot].process_byte_send = 0;
	server->player[slot].last_ping_sent = 0;

	messageSend(socket, 0, MSG_SEND_REQUEST_IDENTIFY, slot, 0, NULL);

	return 0;
}
