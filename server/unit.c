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



int unitShieldMax(int type) {
	if (type < 0 || type > UNITS_DEFINED)
		return 0;
	return unit_maxshield[type];
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
	unit->pylon.pulse = 0;
	unit->pylon.power = 0;
	unit->pylon.unit = unit;
	unit->pylon.next = NULL;

	for (j = -1 * radius; j <= radius; j++) {
		if (j + x < 0 || j + x >= server->w)
			continue;
		for (k = -1 * radius; k <= radius; k++) {
			if (k + y < 0 || k + y >= server->w)
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
			if (server->map[index]->pylon.power && !unit->pylon.power) {
				playerCalcSetPower(owner, x, y, 1);
				fprintf(stderr, "Setting power...\n");
				unit->pylon.power = 1;
			} else if (!server->map[index]->pylon.power) 
				fprintf(stderr, "Pylon at %i %i is unpowered\n", j + x, k + y);

			unitPylonListAdd(&unit->pylon.next, &server->map[index]->pylon);
			unitPylonListAdd(&server->map[index]->pylon.next, &server->map[index]->pylon);
			unitPylonListAdd(&server->pylons, &unit->pylon);
		}
	}
	
	unit->pylon.power = (unit->type == UNIT_DEF_GENERATOR) ? 1 : unit->pylon.power;

	return;
}


void unitAnnounce(int from, int to, int building, int index) {
	int progress;

	messageBufferPushDirect(to, from, MSG_SEND_BUILDING_PLACE, building, index, NULL);

	if (building && building != UNIT_DEF_BUILDSITE_FREE) {
		progress = server->map[index]->hp * 100 / unitHPMax(building);
		messageBufferPushDirect(from, to, MSG_SEND_BUILDING_HP, progress, 0, NULL);
		progress = server->map[index]->shield * 100 / unitShieldMax(building);
		messageBufferPushDirect(from, to, MSG_SEND_BUILDING_SHIELD, progress, 0, NULL);
		if (server->map[index]->target)
			messageBufferPushDirect(from, to, MSG_SEND_BUILDING_ATTACKING, index, server->map[index]->target, NULL);
	}

	return;
}


int unitSpawn(unsigned int player, unsigned int unit, unsigned int x, unsigned int y) {
	unsigned int index;
	int i, team;

	team = server->player[player].team;
	index = x + server->w * y;
	if (server->map[index])
		return -1;
	if (!server->player[player].map[index].fog && unit != UNIT_DEF_GENERATOR)
		return -1;
	if (unitAdd(player, unit, x, y) >= 0)
		playerBuildQueueStop(player, unit);

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		if (server->player[i].team == team && team != -1)
			continue;
		if (i == player)
			continue;
		if (server->player[i].map[index].fog) {
			unitAnnounce(player, i, unit, index);
		}
	}

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
	unit->shield = unitShieldMax(unit->type);		/* Don't spawn with any shield! */
	unit->status = 0;				/* Nothing yet? */
	unit->next = NULL;
	unit->target = -1;
	unit->x = x;
	unit->y = y;
	server->map[x + y * server->w] = unit;
	
	if (unit->type == UNIT_DEF_PYLON || unit->type == UNIT_DEF_GENERATOR)
		unitPylonInit(unit, x, y);
	else
		unit->pylon.next = NULL;

	return unit;
}


int unitAdd(int owner, int type, int x, int y) {
	SERVER_UNIT *unit;
	int index = x + server->w * y;
	
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

	if (server->map_c.tile_data[index] & 0x10000)
		return -1;
	if ((server->map_c.tile_data[index] & 0xFFF) == UNIT_BUILDSITE && unit->type != UNIT_DEF_BUILDSITE)
		return -1;
	if (unit->type == UNIT_DEF_BUILDSITE) {
		if ((server->map_c.tile_data[index] & 0xFFF) != UNIT_BUILDSITE)
			return -1;
	}
	
	unit->next = server->unit;
	server->unit = unit;

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
	parent = &server->unit;
	next = *parent;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		if (!server->player[i].map[x + y * server->h].fog)
			continue;
		messageBufferPushDirect(i, unit->owner, MSG_SEND_BUILDING_PLACE, 0, x + server->w * y, NULL);
	}

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
			fprintf(stderr, "Generator lost! Player needs to die!\n");
			playerMessageBroadcast(next->owner, MSG_SEND_PLAYER_DEFEATED, 0, 0, NULL);
			/* TODO: Handle player loss */
		}
		free(next);
		server->map[x + y * server->h] = NULL;
		return 0;
	}
	
	server->map[x + y * server->h] = NULL;

	return 0;
}


void unitDestroy(int player, unsigned int index) {
	if (index > server->w * server->h)
		return;
	if (server->map[index]->owner != player)
		return;
	if (server->map[index]->type == UNIT_DEF_GENERATOR)
		return;
	unitRemove(index % server->w, index / server->w);

	return;
}


void unitAttackSet(int index_src, int index_dst) {
	int i, from;

	from = server->map[index_src]->owner;
	server->map[index_src]->target = index_dst;
	fprintf(stderr, "Attacking %i->%i\n", index_src, index_dst);

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		if (!server->player[i].map[index_src].fog && !server->player[i].map[index_dst].fog)
			continue;
		messageBufferPushDirect(i, from, MSG_SEND_BUILDING_ATTACKING, index_src, index_dst, NULL);
	}


	return;
}


void unitDamageAnnounce(int index) {
	int i, hp;

	hp = server->map[index]->hp * 100 / unitHPMax(server->map[index]->type);

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		if (server->player[i].map[index].fog)
			messageBufferPushDirect(server->map[index]->owner, i, MSG_SEND_BUILDING_HP, hp, index, NULL);
	}

	return;
}


int unitAttackValidate(int index_src, int owner, int index_dst) {
	int team;

	team = server->player[owner].team;

	if (index_src < 0 || index_src >= server->w * server->h)
		return -1;
	if (index_dst < 0 || index_dst >= server->w * server->h)
		return -1;
	if (!server->map[index_dst])
		return -1;
	if (!server->map[index_src])
		return -1;
	if (server->map[index_src]->owner != owner)
		return -1;
	if (!server->player[owner].map[index_dst].fog)
		return -1;
	if (server->map[index_dst]->owner == owner)
		return -1;
	if (team > -1 && server->player[server->map[index_dst]->owner].team == team)
		return -1;
	return 0;
}


void unitAttackerScan(int x, int y) {
	int team, i, j, index, range, owner;
	
	range = unitRange(UNIT_DEF_ATTACKER);
	team = server->player[server->map[x + y * server->w]->owner].team;
	owner = server->map[x + y * server->w]->owner;

	for (i = -1 * range; i <= range; i++) {
		if ((i + x) < 0 || (i + x) >= server->w)
			continue;
		for (j = -1 * range; j <= range; j++) {
			if ((j + y) < 0 || (j + y) >= server->h)
				continue;
			index = (j + y) * server->w + (i + x);
			if (!server->map[index])
				continue;
			if (!server->player[owner].map[index].fog)
				continue;
			if (server->map[index]->owner == owner)
				continue;
			if (team > -1 && server->player[server->map[index]->owner].team == team)
				continue;
			fprintf(stderr, "Attacking building, %i\n", index);
			unitAttackSet(x + y * server->w, index);
			return;
		}
	}

	return;
}


void unitShieldAnnounce(int index) {
	int i, shield;

	shield = server->map[index]->shield * 100 / unitShieldMax(server->map[index]->type);

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		if (server->player[i].map[index].fog)
			messageBufferPushDirect(server->map[index]->owner, i, MSG_SEND_BUILDING_SHIELD, shield, index, NULL);
	}

	fprintf(stderr, "Unit at %i has sheild %i\n", index, shield);
		
	return;
}


void unitLoop(int msec) {
	SERVER_UNIT *next;
	int index;

	next = server->unit;
	while (next != NULL) {
		index = next->x + next->y * server->w;
		if (next->shield < unit_maxshield[next->type] && server->player[next->owner].map[index].power) {
			if (next->last_no_shield >= server->game.time_elapsed + UNIT_REGEN_DELAY/server->game.gamespeed) {
				next->shield += unit_shieldreg[next->type] * msec * server->game.gamespeed;
				if (next->shield > unit_maxshield[next->type])
					next->shield = unit_maxshield[next->type];
			}
			
			unitShieldAnnounce(index);
		}
		
		if (next->type == UNIT_DEF_ATTACKER && next->target > -1) {
			server->map[next->target]->shield -= UNIT_ATTACKER_DMGP * msec * server->game.gamespeed;
			if (server->map[next->target]->shield < 0) {
				server->map[next->target]->hp += server->map[next->target]->shield;
				server->map[next->target]->shield = 0;
				server->map[next->target]->last_no_shield = server->game.time_elapsed;
				unitDamageAnnounce(next->target);
				if (server->map[next->target]->hp <= 0) {
					unitRemove(next->target % server->w, next->target / server->w);
					next->target = -1;
				}
			}

		} else if (next->type == UNIT_DEF_ATTACKER) {
			unitAttackerScan(next->x, next->y);
		}

		next = next->next;
	}

	return;
}
