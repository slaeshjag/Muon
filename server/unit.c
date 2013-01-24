/* Muon - a new kind of rts
 * Copyright 2012 Steven Arnow <s@rdw.se> and Axel Isaksson (mr3d/h4xxel)
 * 
 * This file is part of Muon.
 * 
 * Muon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Muon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Muon.  If not, see <http://www.gnu.org/licenses/>.
 */

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
	i = dx + dy * (range * 2 + 1);

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
			unitPylonPulseClimb(pylon->neighbour[i]);

	return;
}


void unitPylonPulse() {
	int i;
	UNIT_PYLON *list;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME_NOW)
			continue;
		if (!server->map[server->player[i].spawn.index])
			return;
		if (server->map[server->player[i].spawn.index]->type != UNIT_DEF_GENERATOR)
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

	if (server->pylons == NULL)
		return;

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
			if (k * k + j * j > radius*radius)
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
		if (server->map[index]->target > -1)
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
	if (unitAdd(player, unit, x, y) < 0)
		return -1;
	server->player[player].queue.queue.ready = 0;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_GAME_NOW)
			continue;
		if (server->player[i].team == team && team != -1)
			continue;
		if (i == player && server->player[i].status == PLAYER_IN_GAME_NOW)
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
	unit->shield = unitShieldMax(unit->type);		/* -Don't-spawn-with-any-shield!- ;; Now we do! */
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

	if ((server->map_c.tile_data[index] & 0xFFF) == UNIT_BUILDSITE && type < UNIT_DEF_BUILDSITE)
		return -1;
	if (server->map_c.tile_data[index] & 0x10000)
		return -1;

	if ((unit = unitInit(owner, type, x, y)) == NULL)
		return -1;

	if (type >= UNIT_DEF_BUILDSITE) {
		if (controlpointNew(unit) < 0) {
			free(unit);
			server->map[x + y * server->w] = NULL;
			return -1;
		}
	}
	
	if (server->player[owner].status == PLAYER_IN_GAME_NOW) {
		server->player[owner].stats.buildings_raised++;
		server->player[owner].stats.points += unit_points[unit->type];
		server->player[owner].stats.points_visible += unit_points[unit->type];
		fprintf(stderr, "Player %i now has %i points (+%i)\n", owner, server->player[owner].stats.points_visible, unit_points[unit->type]);
	}
	
	unit->next = server->unit;
	server->unit = unit;
	server->map_c.tile_data[index] |= 0x80000;
	
	playerCalcLOS(owner, x , y, 1);
	if (type == UNIT_DEF_GENERATOR)
		playerCalcSetPower(owner, x, y, 1);

	return 0;
}


int unitRemove(int x, int y, int who) {
	SERVER_UNIT *unit, *next, **parent;
	int i, owner, type, index;

	if (x >= server->w || y >= server->h) {
		fprintf(stderr, "Unable to remove unit at %i %i: Tile is outside of the map\n", x, y);
		return -1;
	}

	index = x + y * server->w;
	if ((unit = server->map[index]) == NULL)
		return -1;
	parent = &server->unit;
	next = *parent;
	owner = unit->owner;
	unit->hp = 0;
	unit->shield = 0;
	type = ((server->map_c.tile_data[index] & 0xFFF) == UNIT_BUILDSITE) ? UNIT_DEF_BUILDSITE_FREE : 0;

	playerCalcLOS(unit->owner, x, y, -1);
	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME_NOW)
			continue;
		if ((!server->player[i].map[index].fog) && unit->owner != i)
			continue;
		messageBufferPushDirect(i, unit->owner, MSG_SEND_BUILDING_PLACE, type, index, NULL);
		messageBufferPushDirect(i, unit->owner, MSG_SEND_BUILDING_HP, 0, index, NULL);
		messageBufferPushDirect(i, unit->owner, MSG_SEND_BUILDING_SHIELD, 0, index, NULL);
	}
	
	while (next != unit) {
		parent = &next->next;
		next = next->next;
	}
	
	if (server->player[owner].status == PLAYER_IN_GAME_NOW) {
		server->player[owner].stats.buildings_lost++;
		server->player[owner].stats.points -= unit_points[unit->type];
		if (who != owner)
			server->player[who].stats.points_visible += unit_points[unit->type];
	}
	
	
	if (next == unit) {
		*parent = next->next;
		if (next->type == UNIT_DEF_PYLON) {
			unitPylonDelete(next);
			unitPylonPulse();
		} else if (next->type == UNIT_DEF_GENERATOR) {
			server->player[next->owner].status = PLAYER_SPECTATING;
			playerMessageBroadcast(next->owner, MSG_SEND_PLAYER_DEFEATED, 0, 0, NULL);
			unitDestroyAll(next->owner);
			playerDefeatAnnounce(next->owner);
			unitPylonDelete(next);
			server->map[x + y * server->h] = NULL;
			if (gameDetectIfOver() == 0)
				gameEnd();
		} else if (next->type >= UNIT_DEF_BUILDSITE) {
			controlpointRemove(next);
		}
		free(next);
		server->map[x + y * server->h] = NULL;
	}
	
	server->map[x + y * server->h] = NULL;

	return 0;
}


int unitWallTest(int index_src, int index_dst, int player) {
	int p, x1, x2, y1, y2, dx, dy, x, y, y_dir, end, index, team, y_max;

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
	y_max = (y1 < y2) ? y2 : y1;
	end = (x1 < x2) ? x2 : x1;

	y_dir = (y < y_max) ? 1 : -1;

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
			if (i*i + j*j > range*range)
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


void unitDestroy(int player, unsigned int index, int who) {
	if (index > server->w * server->h)
		return;
	if (!server->map[index])
		return;
	if (server->map[index]->owner != player)
		return;
	unitRemove(index % server->w, index / server->w, who);

	return;
}


void unitAttackSet(int index_src, int index_dst) {
	int i, from, range, x, y, tx, ty;

	if (index_dst >= 0) {
		range = unit_range[server->map[index_src]->type];
		x = index_src % server->w;
		y = index_src / server->w;
		tx = index_dst % server->w;
		ty = index_dst / server->h;
		if ((x - tx) * (x - tx) + (y - ty) * (y - ty) >= range * range)
			return;
	}

	from = server->map[index_src]->owner;
	server->map[index_src]->target = (index_dst > -1) ? index_dst : -1;
	
	/* We might need this later */
	/*fprintf(stderr, "Attacking %i->%i\n", index_src, index_dst);*/
	if (index_dst == -1)
		index_dst = -1;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME_NOW)
			continue;
		if (!server->player[i].map[index_src].fog && !server->player[i].map[index_dst].fog)
			continue;
		messageBufferPushDirect(i, from, MSG_SEND_BUILDING_ATTACKING, index_dst, index_src, NULL);
	}


	return;
}


void unitDamageAnnounce(int index) {
	int i, hp;

	hp = server->map[index]->hp * 100 / unitHPMax(server->map[index]->type);
	if (!hp && server->map[index]->hp)
		hp = 1;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_GAME_NOW)
			continue;
		if (server->player[i].map[index].fog || server->player[i].status == PLAYER_SPECTATING)
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
	int team, i, j, index, range, owner, target_x, target_y;
	
	range = unit_range[server->map[server->w * y + x]->type];
	team = server->player[server->map[x + y * server->w]->owner].team;
	owner = server->map[x + y * server->w]->owner;
	target_x = target_y = 0x7FFFFFFF;

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
			if (target_x * target_x + target_y * target_y > i * i + j * j || target_x == 0x7FFFFFFF) {
				target_x = i;
				target_y = j;
			}
		}
	}


	if (target_x != 0x7FFFFFFF) {
		target_x += x;
		target_y += y;
		unitAttackSet(x + y * server->w, target_x + target_y * server->w);
	}

	return;
}


void unitShieldAnnounce(int index) {
	int i, shield;

	shield = server->map[index]->shield * 100 / unitShieldMax(server->map[index]->type);

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_GAME_NOW)
			continue;
		if (server->player[i].map[index].fog || server->player[i].status == PLAYER_SPECTATING)
			messageBufferPushDirect(i, server->map[index]->owner, MSG_SEND_BUILDING_SHIELD, shield, index, NULL);
		
	}

	return;
}


/* This is probably *THE* most evil hack in Muon's entire codebase */
void unitDamagePoke(int index, int damage) {
	int gamespeed;

	gamespeed = server->game.gamespeed;
	server->game.gamespeed = 1;
	unitDamageDo(index, damage, 1);
	server->game.gamespeed = gamespeed;

	return;
}


void unitDamageDo(int index, int damage, int time) {
	SERVER_UNIT *next = server->map[index];
	int owner = next->owner;
	int i;
	
	if (next->target < 0 || next->target > server->w * server->h)
		return;

	if (!server->map[next->target]) {
		if (damage < MAP_TERRAIN_ABSORTION)
			return;
		server->map_c.tile_data[next->target] &= 0xF0FFF;
		server->map_c.tile_data[next->target] |= 0x70000;
		for (i = 0; i < server->players; i++) {
			if (server->player[i].status == PLAYER_UNUSED)
				continue;
			if (!server->player[i].map[next->target].fog)
				continue;

			messageBufferPushDirect(i, owner, MSG_SEND_MAP_TILE_ATTRIB, 0x10, next->target, NULL);
		}

		return;
	}


	server->map[next->target]->shield -= damage * time * server->game.gamespeed;
	if (server->map[next->target]->shield < 0) {
		server->map[next->target]->hp += server->map[next->target]->shield;
		server->map[next->target]->shield = 0;
		server->map[next->target]->last_no_shield = server->game.time_elapsed;
		unitDamageAnnounce(next->target);
		if (!server->player[server->map[next->target]->owner].map[next->target].power)
			unitShieldAnnounce(next->target);
		if (server->map[next->target]->hp <= 0) {
			server->player[next->owner].stats.buildings_destroyed++;
			unitRemove(next->target % server->w, next->target / server->w, next->owner);
			next->target = -1;
		}
	}

	return;
}
	


void unitLoop(int msec) {
	SERVER_UNIT *next;
	int index;

	next = server->unit;
	while (next != NULL) {
		index = next->x + next->y * server->w;
		if (next->shield > 0 && !server->player[next->owner].map[index].power) {
			next->shield = 0;
			unitShieldAnnounce(index);
		}

		if (next->shield < unit_maxshield[next->type] && server->player[next->owner].map[index].power) {
			if (next->last_no_shield + UNIT_REGEN_DELAY/server->game.gamespeed <= server->game.time_elapsed) {
				next->shield += unit_shieldreg[next->type] * msec * server->game.gamespeed;
				if (next->shield > unit_maxshield[next->type])
					next->shield = unit_maxshield[next->type];
			}
			unitShieldAnnounce(index);
		}
		
		if ((next->type == UNIT_DEF_ATTACKER || next->type == UNIT_DEF_SCOUT) && next->target > -1) {
			if ((!server->player[next->owner].map[next->target].fog) || (!server->map[next->target])) {
				unitAttackSet(index, index);
				next->target = -1;
			} else {
				unitDamageDo(index, unit_damage[next->type], msec);
			}
		} else if ((next->type == UNIT_DEF_ATTACKER || next->type == UNIT_DEF_SCOUT)) {
			unitAttackerScan(next->x, next->y);
		}

		next = next->next;
	}

	controlpointLoop(msec);

	return;
}


void unitDestroyAll(int player) {
	int i;

	for (i = 0; i < (const int) server->w * (const int) server->h; i++) {
		if (!server->map[i])
			continue;
		if (server->map[i]->owner != player)
			continue;
		if (server->map[i]->type == UNIT_DEF_GENERATOR)
			continue;
		unitRemove(i % server->w, i / server->w, player);
	}

	return;
}


void unitAnnounceBuildingData(int player) {
	int i;

	for (i = 1; i <= UNITS_DEFINED; i++) {
		messageBufferPushDirect(player, player, MSG_SEND_UNIT_ATTACK, i, unit_damage[i], NULL); 
		messageBufferPushDirect(player, player, MSG_SEND_UNIT_RANGE, i, unit_range[i], NULL); 
		messageBufferPushDirect(player, player, MSG_SEND_UNIT_LOS, i, unit_los[i], NULL); 
	}

	return;
}
