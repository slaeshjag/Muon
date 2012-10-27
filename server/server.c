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


int serverLoop(unsigned int d_ms) {
	/* FIXME: STUB */
	if (!server->game.started)
		lobbyPoll();

	return 0;
}

