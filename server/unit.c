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


int unitNeighbours(const int radius) {
	/* This will waste a lot of memory, but I just want to get this shit to work */
	return (radius*2+1)*(radius*2+1);
}


int unitPylonListRemove(UNIT_PYLON *from, UNIT_PYLON *del) {
	int i;

	for (i = 0; i < from->neighbours; i++)
		if (from->neighbour[i] == del) {
			from->neighbour[i] = NULL;
			return 0;
		}
	return -1;
}



int unitPylonListAdd(UNIT_PYLON *to, UNIT_PYLON *add) {
	int i, range, dx, dy;

	dx = add->x - to->x;
	dy = add->y - to->y;
	range = unit_range[to->unit->type];
	dx += range;
	dy += range;
	i = dx + dy * (range + 1);

	to->neighbour[i] = add;

	return 0;
}


void unitPylonPulseClimb(UNIT_PYLON *pylon) {
	int i;
	
	if (pylon->pulse)
		return;
	pylon->pulse = 1;
	
	for (i = 0; i < pylon->neighbours; i++)
		if (pylon->neighbour[i])
			unitPylonPulseClimb(pylon->neighbour[i]);;

	return;
}


void unitPylonPulse() {
	int i;
	UNIT_PYLON *list;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		unitPylonPulseClimb(&server->map[server->player[i].spawn.index]->pylon);
	}

	list = server->pylons;
	
	while (list != NULL) {
		if (list->pulse) {
			if (list->power);
			else {
				list->power = 1;
				playerCalcSetPower(list->unit->owner, list->x, list->y, 1);
			}
		} else {
			if (list->power) {
				list->power = 0;
				playerCalcSetPower(list->unit->owner, list->x, list->y, -1);
			}
		}
		list->pulse = 0;

		list = list->next;
	}

	return;
}


void unitPylonDelete(SERVER_UNIT *unit) {
	UNIT_PYLON **list;
	int i;

	if (unit->pylon.power)
		playerCalcSetPower(unit->owner, unit->pylon.x, unit->pylon.y, -1);
	
	for (i = 0; i < unit->pylon.neighbours; i++)
		if (unit->pylon.neighbour[i])
			unitPylonListRemove(unit->pylon.neighbour[i], &unit->pylon);
	free(unit->pylon.neighbour);
	unit->pylon.neighbour = NULL;

	list = &server->pylons;

	while (*list) {
		if ((*list)->unit == unit) {
			*list = (*list)->next;
			return;
		}

		list = &(*list)->next;
	}

	return;
}


void unitPylonInit(SERVER_UNIT *unit, unsigned int x, unsigned int y) {
	int i, j, k, owner, team, radius, index;

	owner = unit->owner;
	team = server->player[owner].team;
	radius = unitRange(unit->type);

	unit->pylon.x = x;
	unit->pylon.y = y;
	unit->pylon.pulse = 0;
	unit->pylon.power = 0;
	unit->pylon.unit = unit;
	unit->pylon.neighbours = unitNeighbours(unitRange(unit->type));
	unit->pylon.neighbour = malloc(sizeof(UNIT_PYLON **) * unit->pylon.neighbours);

	for (i = 0; i < unit->pylon.neighbours; i++)
		unit->pylon.neighbour[i] = NULL;

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
				unit->pylon.power = 1;
			}

			unitPylonListAdd(&server->map[index]->pylon, &unit->pylon);
			unitPylonListAdd(&unit->pylon, &server->map[index]->pylon);
		}
	}
	
	unit->pylon.power = (unit->type == UNIT_DEF_GENERATOR) ? 1 : unit->pylon.power;
	unit->pylon.next = server->pylons;
	server->pylons = &unit->pylon;

	unitPylonPulse();

	return;
}


void unitAnnounce(int from, int to, int building, int index) {
	int progress;

	messageBufferPushDirect(to, from, MSG_SEND_BUILDING_PLACE, building, index, NULL);

	if (building && building != UNIT_DEF_BUILDSITE_FREE) {
		progress = server->map[index]->hp * 100 / unitHPMax(building);
		messageBufferPushDirect(to, from, MSG_SEND_BUILDING_HP, progress, index, NULL);
		progress = server->map[index]->shield * 100 / unitShieldMax(building);
		messageBufferPushDirect(to, from, MSG_SEND_BUILDING_SHIELD, progress, index, NULL);
		if (server->map[index]->target)
			messageBufferPushDirect(to, from, MSG_SEND_BUILDING_ATTACKING, index, server->map[index]->target, NULL);
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
	unit->last_no_shield = 0;
	
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
	int i;

	if (x >= server->w || y >= server->h) {
		fprintf(stderr, "Unable to remove unit at %i %i: Tile is outside of the map\n", x, y);
		return -1;
	}

	if ((unit = server->map[x + y * server->h]) == NULL)
		return -1;
	parent = &server->unit;
	next = *parent;

	playerCalcLOS(unit->owner, unit->x, unit->y, -1);
	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		if (!server->player[i].map[x + y * server->w].fog)
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


int unitWallTest(int index_src, int index_dst, int player) {
	int p, x1, x2, y1, y2, dx, dy, x, y, y_dir, end, index, team;

	team = server->player[player].team;

	x1 = index_src % server->w;
	x2 = index_dst % server->w;
	y1 = index_src / server->w;
	y2 = index_dst / server->w;
	
	dx = abs(x1 - x2);
	dy = abs(y1 - y2);
	p = 2 * dy - dx;

	x = (x1 < x2) ? x1 : x2;
	y = (x1 < x2) ? y1 : y2;
	end = (x1 < x2) ? x2 : x1;

	y_dir = (y < y2) ? 1 : -1;

	while (x < end) {
		x++;
		if (p < 0)
			p += 2 * dy;
		else {
			y += y_dir;
			p += 2 * (dy - dx);
		}

		index = x + y * server->w;
		if (server->map[index])
			if (server->map[index]->type == UNIT_DEF_WALL) {
				if (team > -1 && server->player[player].team != team)
					return -1;
				if (team == -1 && server->map[index]->owner != player)
					return -1;
				continue;
			}
	}

	return 0;
}


int unitValidateWall(int index, int player) {
	int i, j, range, team, x, y, index_dst;

	x = index % server->w;
	y = index / server->w;
	team = server->player[player].team;
	range = unitRange(UNIT_DEF_PYLON);

	for (i = -1*range; i <= range; i++) {
		if (x + i < 0 || x + i >= server->w)
			continue;
		for (j = -1*range; j <= range; j++) {
			if (y + j < 0 || y + j >= server->w)
				continue;
			if ((x + i)*(x + i) + (y + j)*(y + j) > range*range)
				continue;

			index_dst = (x + i) + (y + j) * server->w;
			if (!server->map[index_dst])
				continue;
			if (server->map[index_dst]->type != UNIT_DEF_GENERATOR && server->map[index_dst]->type != UNIT_DEF_PYLON)
				continue;
			if (team > -1 && server->player[server->map[index_dst]->owner].team != team)
				continue;
			if (server->map[index_dst]->owner != player && team == -1)
				continue;
			if (unitWallTest(index, index_dst, player) == 0)
				return 0;
		}
	}

	return -1;
}


void unitDestroy(int player, unsigned int index) {
	if (index > server->w * server->h)
		return;
	if (!server->map[index])
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
	if (!hp && server->map[index]->hp)
		hp = 1;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		if (server->player[i].map[index].fog)
			messageBufferPushDirect(i, server->map[index]->owner, MSG_SEND_BUILDING_HP, hp, index, NULL);
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
			messageBufferPushDirect(i, server->map[index]->owner, MSG_SEND_BUILDING_SHIELD, shield, index, NULL);
		
	}

	return;
}


void unitLoop(int msec) {
	SERVER_UNIT *next;
	int index;

	next = server->unit;
	while (next != NULL) {
		index = next->x + next->y * server->w;
		if (next->shield < unit_maxshield[next->type] && server->player[next->owner].map[index].power) {
			if (next->last_no_shield + UNIT_REGEN_DELAY/server->game.gamespeed <= server->game.time_elapsed) {
				next->shield += unit_shieldreg[next->type] * msec * server->game.gamespeed;
				if (next->shield > unit_maxshield[next->type])
					next->shield = unit_maxshield[next->type];
			}
			
			unitShieldAnnounce(index);
		}
		
		if (next->type == UNIT_DEF_ATTACKER && next->target > -1) {
			if (!server->map[next->target])
				next->target = -1;
			else {
				server->map[next->target]->shield -= UNIT_ATTACKER_DMGP * msec * server->game.gamespeed;
				if (server->map[next->target]->shield < 0) {
					server->map[next->target]->hp += server->map[next->target]->shield;
					server->map[next->target]->shield = 0;
					server->map[next->target]->last_no_shield = server->game.time_elapsed;
					unitDamageAnnounce(next->target);
					if (!server->player[server->map[next->target]->owner].map[next->target].power)
						unitShieldAnnounce(index);
					if (server->map[next->target]->hp <= 0) {
						unitRemove(next->target % server->w, next->target / server->w);
						next->target = -1;
					}
				}
			}
		} else if (next->type == UNIT_DEF_ATTACKER) {
			unitAttackerScan(next->x, next->y);
		}

		next = next->next;
	}

	return;
}
