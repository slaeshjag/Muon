#include "server.h"


int lobbyPoll(SERVER *server) {
	SERVER_SOCKET *socket;
	int slot;

	if ((socket = networkAccept(server->accept)) == NULL)
		return 0;
	if ((slot = playerSlot(server)) == -1) {
		messageSend(socket, 0, MSG_SERVER_FULL, 0, 0, NULL);
		networkSocketDisconnect(socket);
		return 0;
	}

	server->player[slot].status = PLAYER_WAITING_FOR_IDENTIFY;
	server->player[slot].socket = socket;
	
	messageSend(socket, 0, MSG_REQUEST_IDENTIFY, 0, 0, NULL);

	return 0;
}
