#include "server.h"


int unitLOS(int type) {
	if (type < 0 || type > UNITS_DEFINED)
		return 0;
	return unit_los[type];
}


int unitHPMax(int type) {
	if (type < 0 || type > UNITS_DEFINED)
		return 0;
	return unit_maxhp[type];
}


int unitRange(int type) {
	if (type < 0 || type > UNITS_DEFINED)
		return 0;
	return unit_range[type];
}


int unitSpawn(unsigned int player, unsigned int unit, unsigned int x, unsigned int y) {
	unsigned int index;

	index = x + server->w * y;
	if (server->map[index])
		return -1;
	if (server->player[player].map[index].fog && unit != UNIT_GENERATOR)
		return -1;
	unitAdd(player, unit, x, y);

	return 0;
}
	

SERVER_UNIT *unitInit(int owner, int type, int x, int y) {
	SERVER_UNIT *unit;

	if ((unit = malloc(sizeof(SERVER_UNIT))) == NULL) {
		errorPush(SERVER_ERROR_NO_MEMORY);
		fprintf(stderr, "Unable to alloc unit\n");
		return NULL;
	}

	unit->owner = owner;
	unit->type = type;
	unit->hp = unitHPMax(type);
	unit->shield = 0;		/* Don't spawn with any shield! */
	unit->powered = serverPowerGet(owner, x, y);
	unit->status = 0;		/* Nothing yet? */
	unit->updating = 0;
	unit->next = NULL;


	return unit;
}


int unitAdd(int owner, int type, int x, int y) {
	SERVER_UNIT *unit;
	
	if (x >= server->w || y >= server->h) {
		fprintf(stderr, "Warning: Unable to create unit at %i %i: Coordinate is outside the map\n", x, y);
		return -1;
	}

	if (server->map[x + y * server->w]) {
		fprintf(stderr, "Warning: Unable to create unit at %i %i: Tile is occupied\n", x, y);
		return -1;
	}

	if ((unit = unitInit(owner, type, x, y)) == NULL)
		return -1;
	
	unit->next = server->unit;
	server->unit = unit;

	server->map[x + y * server->w] = unit;
	playerCalcLOS(server->player[owner].team, owner, x + y * server->w, 1);
	if (type == UNIT_GENERATOR)
		playerCalcSetPower(server->player[owner].team, owner, x + y * server->w, 1);

	return 0;
}


int unitRemove(int x, int y) {
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
