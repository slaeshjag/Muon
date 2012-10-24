#include "server.h"


/* TODO: Expand with arguments */
SERVER *serverInit(const int map_w, const int map_h) {
	int i;
	SERVER *server;

	if ((server = malloc(sizeof(SERVER))) == NULL) {
		fprintf(stderr, "Unable to allocate a server\n");
		return NULL;
	}

	if ((server->map = malloc(sizeof(SERVER_UNIT *) * map_w * map_h)) == NULL) {
		fprintf(stderr, "Unable to allocate a server\n");
		free(server);
		return NULL;
	}

	for (i = 0; i < map_w * map_h; i++)
		server->map[i] = NULL;
	
	server->unit = NULL;
	server->w = map_w;
	server->h = map_h;

	return server;
}


SERVER *serverDestroy(SERVER *server) {
	int i;

	for (i = 0; i < server->w * server->h; i++)
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
