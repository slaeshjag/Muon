#include "server.h"


int serverPowerGet(int owner, int x, int y) {
	return 1;
}


/* TODO: Expand with arguments */
SERVER *serverInit(const int map_w, const int map_h, unsigned int players) {
	int i;
	SERVER *server;

	if ((server = malloc(sizeof(SERVER))) == NULL) {
		fprintf(stderr, "Unable to allocate a server\n");
		return NULL;
	}

	server->map = malloc(sizeof(SERVER_UNIT *) * map_w * map_h);
	server->message_buffer = messageBufferInit();

	if (!server->map || !server->message_buffer) {
		fprintf(stderr, "Unable to allocate a server\n");
		messageBufferDelete(server->message_buffer);
		free(server);
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


SERVER *serverDestroy(SERVER *server) {
	int i;

	for (i = 0; i < server->w * server->h; i++) {
		messageBufferDelete(server->message_buffer);
		free(server->map[i]);
		free(server->map);
		free(server);
	}

	return NULL;
}


int serverLoop(SERVER *server) {
	/* FIXME: STUB */

	return 0;
}


int main(int argc, char **argv) {
	/* FIXME: Do stuff */
	return 0;
}
