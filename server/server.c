#include "server.h"


int serverPowerGet(int owner, int x, int y) {
	return server->player[owner].map[x + y * server->w].power;
}


/* TODO: Expand with arguments */
SERVER *serverInit(const char *fname, unsigned int players, int port) {
	int i, map_w, map_h;

	if ((server = malloc(sizeof(SERVER))) == NULL) {
		fprintf(stderr, "Unable to allocate a server\n");
		return NULL;
	}

	messageHandlerInit(server);
	gameInit();

	if ((server->map_data = ldmzLoad(fname)) == NULL) {
		free(server);
		server = NULL;
		fprintf(stderr, "Unable to load map %s\n", fname);
		return NULL;
	}

	ldmzGetSize(server->map_data, &map_w, &map_h);
	server->map = malloc(sizeof(SERVER_UNIT *) * map_w * map_h);
	server->message_buffer = messageBufferInit();
	server->player = playerInit(players, map_w, map_h);
	server->accept = networkListen(port);

	if (!server->map || !server->message_buffer || !server->player || !server->accept) {
		fprintf(stderr, "Unable to allocate a server\n");
		messageBufferDelete(server->message_buffer);
		playerDestroy(server->player, server->players);
		networkSocketDisconnect(server->accept);
		ldmzFree(server->map_data);
		free(server);
		server = NULL;
		return NULL;
	}

	for (i = 0; i < map_w * map_h; i++)
		server->map[i] = NULL;
	
	server->unit = NULL;
	server->w = map_w;
	server->h = map_h;
	server->players = players;

	return server;
}


SERVER *serverDestroy() {
	int i;

	for (i = 0; i < server->w * server->h; i++) {
		free(server->map[i]);
	}
	
	playerDestroy(server->player, server->players);
	free(server->map);
	messageBufferDelete(server->message_buffer);
	networkSocketDisconnect(server->accept);
	ldmzFree(server->map_data);

	free(server);

	return NULL;
}


void serverProcessNetwork() {
	int i, t;
	MESSAGE msg, msg_c;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status == PLAYER_UNUSED)
			continue;
		if (server->player[i].process_recv) {
			msg_c = server->player[i].process_msg_recv;
			if ((t = networkReceiveTry(server->player[i].socket, msg_c.extra, msg_c.arg[1])) > 0) {
				server->player[i].process_recv = PLAYER_PROCESS_NOTHING;
				messageExecute(i, &msg_c);
			} else if (t == -1) {
				playerDisconnect(i);
				continue;
			}
		}

		while ((t = networkReceiveTry(server->player[i].socket, (void *) &msg, 16)) > 0) {
			msg.player_ID = ntohl(msg.player_ID);
			msg.command = ntohl(msg.player_ID);
			msg.arg[0] = ntohl(msg.player_ID);
			msg.arg[1] = ntohl(msg.player_ID);

			if (messageHasData(msg.command, msg.arg[1]) <= 0) {
				if (msg.arg[1] > MESSAGE_MAX_PAYLOAD) {
					messageSend(server->player[i].socket, i, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
					playerDisconnect(i);
					break;
				}
				
				if ((msg.extra = malloc(msg.arg[1] + 1)) == NULL) {
					fprintf(stderr, "Fatal error: System seems to be out of RAM. Server is dieing\n");
					serverDestroy();
					exit(-1);
				}

				if ((t = networkReceiveTry(server->player[i].socket, (void *) msg.extra, msg.arg[1])) > 0) {
					messageExecute(i, &msg_c);
				} else {
					server->player[i].process_msg_recv = msg;
					server->player[i].process_recv = PLAYER_PROCESS_DATA;
					break;
				}
			} else {
				messageExecute(i, &msg_c);
			}
		}
	}

	return;
}



int serverLoop(unsigned int d_ms) {
	/* FIXME: STUB */
	if (!server->game.started) {
		lobbyPoll();
		serverProcessNetwork();
		playerCheckIdentify();
	}

	return 0;
}

