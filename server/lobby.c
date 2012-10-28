#include "server.h"


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

	messageSend(socket, 0, MSG_SEND_REQUEST_IDENTIFY, slot, 0, NULL);

	return 0;
}
