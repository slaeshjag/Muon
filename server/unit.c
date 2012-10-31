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


int unitPylonListRemove(UNIT_PYLON_LIST **list, UNIT_PYLON *del) {
	UNIT_PYLON_LIST *tmp;

	while ((*list) != NULL) {
		if ((*list)->pylon == del) {
			tmp = *list;
			*list = (*list)->next;
			free(tmp);
			return 0;
		}
		list = &(*list)->next;
	}

	return -1;
}



int unitPylonListAdd(UNIT_PYLON_LIST **list, UNIT_PYLON *add) {
	UNIT_PYLON_LIST *new;

	if ((new = malloc(sizeof(UNIT_PYLON_LIST))) == NULL) {
		errorPush(SERVER_ERROR_NO_MEMORY);
		return -1;
	}

	new->next = *list;
	new->pylon = add;
	*list = new;

	return 0;
}


void unitPylonPulseClimb(UNIT_PYLON *pylon) {
	UNIT_PYLON_LIST *list;
	if (pylon->pulse)
		return;
	pylon->pulse = 1;
	list = pylon->next;

	while (list != NULL) {
		unitPylonPulseClimb(list->pylon);
		list = list->next;
	}

	return;
}


void unitPylonPulse() {
	int i;
	UNIT_PYLON_LIST *list;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		unitPylonPulseClimb(&server->map[server->player[i].spawn.index]->pylon);
	}
	
	list = server->pylons;
	while (list != NULL) {
		if (list->pylon->pulse) {
			if (list->pylon->power);
			else {
				list->pylon->power = 1;
				playerCalcSetPower(list->pylon->unit->owner, list->pylon->x, list->pylon->y, 1);
			}
		} else {
			if (list->pylon->power) {
				list->pylon->power = 0;
				playerCalcSetPower(list->pylon->unit->owner, list->pylon->x, list->pylon->y, -1);
			}
		}

		list = list->next;
	}

	return;
}


void unitPylonDelete(SERVER_UNIT *unit) {
	UNIT_PYLON_LIST *list, *tmp;

	if (unit->pylon.power)
		playerCalcSetPower(unit->owner, unit->pylon.x, unit->pylon.y, -1);
	
	list = unit->pylon.next;
	unit->pylon.next = NULL;
	
	while (list != NULL) {
		unitPylonListRemove(&list->pylon->next, &unit->pylon);
		tmp = list;
		list = list->next;
		free(tmp);
	}

	return;
}


void unitPylonInit(SERVER_UNIT *unit, unsigned int x, unsigned int y) {
	int j, k, owner, team, radius, index;

	owner = unit->owner;
	team = server->player[owner].team;
	radius = unitRange(unit->type);

	unit->pylon.x = x;
	unit->pylon.y = y;
	unit->pylon.power = 0;
	unit->pylon.pulse = 0;
	unit->pylon.unit = unit;
	unit->pylon.next = NULL;

	for (j = -1 * radius; j <= radius; j++) {
		if (j + x < 0 || j + x > server->w)
			continue;
		for (k = -1 * radius; k <= radius; k++) {
			if (k + y < 0 || k + y > server->w)
				continue;
			index = (y + k) * server->w + (x + j);
			if (!server->map[index])		/* No building here - don't bother */
				continue;
			if (server->map[index]->type != UNIT_DEF_GENERATOR && server->map[index]->type != UNIT_DEF_PYLON)
				/* Wrong type of building. We have no business to do here */
				continue;
			if (team > -1) {
				if (server->player[server->map[index]->owner].team != team)
					/* Building is not on our team */
					continue;
			} else if (server->map[index]->owner != owner)	/* We don't own it */
				continue;
			if (server->map[index]->pylon.power) {
				playerCalcSetPower(owner, j + x, k + y, 1);
				unit->pylon.power = 1;
			}

			fprintf(stderr, "Filling out lists\n");
			unitPylonListAdd(&unit->pylon.next, &server->map[index]->pylon);
			unitPylonListAdd(&server->map[index]->pylon.next, &server->map[index]->pylon);
			unitPylonListAdd(&server->pylons, &unit->pylon);
		}
	}

	return;
}


int unitSpawn(unsigned int player, unsigned int unit, unsigned int x, unsigned int y) {
	unsigned int index;

	index = x + server->w * y;
	if (server->map[index])
		return -1;
	if (!server->player[player].map[index].fog && unit != UNIT_DEF_GENERATOR)
		return -1;
	fprintf(stderr, "Adding unit %i\n", unit);
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
	unit->status = 0;		/* Nothing yet? */
	unit->next = NULL;
	
	if (unit->type == UNIT_DEF_PYLON)
		unitPylonInit(unit, x, y);
	else
		unit->pylon.next = NULL;

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
	playerCalcLOS(owner, x , y, 1);
	if (type == UNIT_DEF_GENERATOR)
		playerCalcSetPower(owner, x, y, 1);

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
		if (next->type == UNIT_DEF_PYLON) {
			unitPylonDelete(next);
			unitPylonPulse();
		} else if (next->type == UNIT_DEF_GENERATOR) {
			/* TODO: Handle player loss */
		}
		free(next);
		return 0;
	}

	return 0;
}
