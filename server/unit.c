#include "server.h"


int unitHPMax(int type) {
	return 1;
}


SERVER_UNIT *unitInit(SERVER *server, int owner, int type, int x, int y) {
	SERVER_UNIT *unit;

	if ((unit = malloc(sizeof(SERVER_UNIT))) == NULL) {
		fprintf(stderr, "Unable to alloc unit\n");
		return NULL;
	}

	unit->owner = owner;
	unit->type = type;
	unit->hp = unitHPMax(type);
	unit->shield = 0;		/* Don't spawn with any shield! */
	unit->powered = serverPowerGet(owner, x, y);
	unit->status = 0;		/* Nothing yet? */
	unit->next = NULL;

	return unit;
}


int unitAdd(SERVER *server, int owner, int type, int x, int y) {
	SERVER_UNIT *unit;
	
	if (x >= server->w || y >= server->h) {
		fprintf(stderr, "Warning: Unable to create unit at %i %i: Coordinate is outside the map\n", x, y);
		return -1;
	}

	if (server->map[x + y * server->w]) {
		fprintf(stderr, "Warning: Unable to create unit at %i %i: Tile is occupied\n", x, y);
		return -1;
	}

	if ((unit = unitInit(server, owner, type, x, y)) == NULL)
		return -1;
	
	unit->next = server->unit;
	server->unit = unit;

	server->map[x + y * server->h] = unit;

	return 0;
}


int unitRemove(SERVER *server, int x, int y) {
	SERVER_UNIT *unit, *next, **parent;

	if (x >= server->w || y >= server->h) {
		fprintf(stderr, "Unable to remove unit at %i %i: Tile is outside of the map\n", x, y);
		return -1;
	}

	if ((unit = server->map[x + y * server->h]) == NULL)
		return -1;
	server->map[x + y * server->h] = NULL;
	parent = &server->unit;
	next = *parent;

	while (next != unit) {
		parent = &next->next;
		next = next->next;
	}

	if (next == unit) {
		*parent = next->next;
		free(next);
		return 0;
	}

	return 0;
}
