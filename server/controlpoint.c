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


int controlpointStructInit(SERVER_UNIT *unit) {
	CONTROLPOINT_EXTRA *new;

	if (!(new = malloc(sizeof(CONTROLPOINT_EXTRA))))
		return -1;
	new->next = (struct CONTROLPOINT_EXTRA *) server->controlpoint;
	server->controlpoint = new;
	unit->cp = new;

	new->type = unit->type;
	new->index = unit->x + unit->y * server->w;
	new->owner = unit->owner;

	return 0;
}


int controlpointClusterbombInit(SERVER_UNIT *unit) {
	return controlpointStructInit(unit);
}


int controlpointRadarInit(SERVER_UNIT *unit) {
	return controlpointStructInit(unit);
}


int controlpointGroundgenInit(SERVER_UNIT *unit) {
	return controlpointStructInit(unit);
}


int controlpointShieldregenInit(SERVER_UNIT *unit) {
	return controlpointStructInit(unit);
}


int controlpointBuildsiteInit(SERVER_UNIT *unit) {
	server->player[unit->owner].buildspots++;
	server->player[unit->owner].buildspeed = CP_BUILDSITE_SPEED(server->player[unit->owner].buildspots);
	unit->cp = NULL;

	return 0;
}


void controlpointInitPlayer(int player) {
	server->player[player].cp.clusterbomb_delay = -1;
	server->player[player].cp.radar_delay = -1;
	server->player[player].cp.radar_deploy = -1;
	server->player[player].cp.radar_pos = -1;
	server->player[player].cp.groundgen_delay = 0;
	server->player[player].cp.shieldregen_delay = 0;

	server->player[player].cp.radar.count = 0;
	server->player[player].cp.radar.speed = 0;
	server->player[player].cp.radar.temp = 0;

	server->player[player].cp.clusterbomb.count = 0;
	server->player[player].cp.clusterbomb.speed = 0;
	server->player[player].cp.clusterbomb.temp = 0;
	
	server->player[player].cp.groundgen.count = 0;
	server->player[player].cp.groundgen.speed = 0;
	server->player[player].cp.groundgen.temp = 0;

	server->player[player].cp.shieldregen.count = 0;
	server->player[player].cp.shieldregen.speed = 0;
	server->player[player].cp.shieldregen.temp = 0;

	return;
}


int controlpointInit() {
	int bombs, i;
	bombs = unit_damage[UNIT_DEF_CLUSTERBOMB] / unit_maxshield[UNIT_DEF_GENERATOR];
	server->controlpoint = NULL;
	server->clusterbomb_buffer = malloc(sizeof(int) * bombs);

	for (i = 0; i < bombs; i++)
		server->clusterbomb_buffer[i] = ~0;

	return 0;
}


int controlpointNew(struct SERVER_UNIT *unit) {

	if ((server->map_c.tile_data[unit->x + unit->y * server->w] & 0xFFF) != UNIT_BUILDSITE)
		return -1;

	switch (unit->type) {
		case UNIT_DEF_BUILDSITE:
			return controlpointBuildsiteInit(unit);
			break;
		case UNIT_DEF_CLUSTERBOMB:
			return controlpointClusterbombInit(unit);
			break;
		case UNIT_DEF_RADAR:
			return controlpointRadarInit(unit);
			break;
		case UNIT_DEF_GROUNDGEN:
			return controlpointGroundgenInit(unit);
			break;
		case UNIT_DEF_SHIELDREGEN:
			return controlpointShieldregenInit(unit);
			break;
		default:
			return 0;
	}

	/* Stub for now */

	return 0;
}


void controlpointBuildsiteRemove(SERVER_UNIT *unit) {
	server->player[unit->owner].buildspots--;
	server->player[unit->owner].buildspeed = CP_BUILDSITE_SPEED(server->player[unit->owner].buildspots);

	return;
}


void controlpointDelayReset(SERVER_UNIT *unit) {
	int *val;
	CONTROLPOINT_EXTRA *next;

	switch (unit->type) {
		case UNIT_DEF_CLUSTERBOMB:
			val = &server->player[unit->owner].cp.clusterbomb_delay;
			break;
		case UNIT_DEF_RADAR:
			val = &server->player[unit->owner].cp.radar_delay;
			break;
		case UNIT_DEF_GROUNDGEN:
			val = &server->player[unit->owner].cp.groundgen_delay;
			break;
		case UNIT_DEF_SHIELDREGEN:
			val = &server->player[unit->owner].cp.shieldregen_delay;
			break;
		default:
			return;
	}

	for (next = server->controlpoint; next; next = (void *) next->next)
		if (next->owner == unit->owner)
			if (next->type == unit->type)
				if (unit->cp != next)
					return;
	*val = -1;
	messageBufferPushDirect(unit->owner, unit->owner, MSG_SEND_CP_CLEAR, unit->type, 0, NULL);

	return;
}


void controlpointClusterbombRemove(SERVER_UNIT *unit) {
	controlpointDelayReset(unit);
	server->player[unit->owner].cp.clusterbomb.count = 0;

	return;
}


void controlpointRadarRemove(SERVER_UNIT *unit) {
	controlpointDelayReset(unit);
	server->player[unit->owner].cp.radar.count = 0;

	return;
}


void controlpointGroundgenRemove(SERVER_UNIT *unit) {
	controlpointDelayReset(unit);
	server->player[unit->owner].cp.groundgen.count = 0;

	return;
}


void controlpointShieldregenRemove(SERVER_UNIT *unit) {
	controlpointDelayReset(unit);
	server->player[unit->owner].cp.shieldregen.count = 0;

	return;
}


void controlpointRemove(struct SERVER_UNIT *unit) {
	CONTROLPOINT_EXTRA *next;
	switch (unit->type) {
		case UNIT_DEF_BUILDSITE:
			controlpointBuildsiteRemove(unit);
			break;
		case UNIT_DEF_CLUSTERBOMB:
			controlpointClusterbombRemove(unit);
			break;
		case UNIT_DEF_RADAR:
			controlpointRadarRemove(unit);
			break;
		case UNIT_DEF_GROUNDGEN:
			controlpointGroundgenRemove(unit);
			break;
		case UNIT_DEF_SHIELDREGEN:
			controlpointShieldregenRemove(unit);
			break;
		default:
			break;
	}

	if (!unit->cp)
		return;
	if (!server->controlpoint) {
		fprintf(stderr, "Internal error: Control point free'd but not in list\n");
		free(unit->cp);
		return;
	}

	if (unit->cp == server->controlpoint)
		server->controlpoint = (void *) unit->cp->next;
	else {
		for (next = server->controlpoint; next->next && (void *) next->next != unit->cp; next = (void *) next->next);
		if (next->next == (void *) unit->cp)
			next->next = unit->cp->next;
	}

	free(unit->cp);

	return;
}


void controlpointDelayLoopConst(int msec, int player) {
	int tmp, *delay, *count, t_tmp, defaults, i;
	float *speed;
	CONTROLPOINT_DATA *cp = &server->player[player].cp;
	CONTROLPOINT_ENTRY *e;

	for (i = UNIT_DEF_CLUSTERBOMB; i <= UNIT_DEF_GROUNDGEN; i++) {
		switch (i) {
			case UNIT_DEF_CLUSTERBOMB:
				e = &cp->clusterbomb;
				delay = &cp->clusterbomb_delay;
				defaults = CP_CLUSTERBOMB_DELAY;
				break;
			case UNIT_DEF_RADAR:
				e = &cp->radar;
				delay = &cp->radar_delay;
				defaults = CP_RADAR_DELAY;
				break;
			case UNIT_DEF_GROUNDGEN:
				e = &cp->groundgen;
				delay = &cp->groundgen_delay;
				defaults = CP_GROUNDGEN_DELAY;
				break;
			case UNIT_DEF_SHIELDREGEN:
				e = &cp->shieldregen;
				delay = &cp->shieldregen_delay;
				defaults = CP_SHIELDREGEN_DELAY;
				break;
			default:
				continue;
		}

		count = &e->count;
		t_tmp = e->temp;
		speed = &e->speed;

		if (!(*delay)) {
			e->temp = 0;
			continue;
		}
		
		if (t_tmp != *count) {
			(*count) = t_tmp;
			if (i == UNIT_DEF_CLUSTERBOMB)
				(*speed) = CP_CLUSTERBOMB_SPEED(*count);
			else if (i == UNIT_DEF_RADAR)
				(*speed) = CP_RADAR_SPEED(*count);
			else if (i == UNIT_DEF_GROUNDGEN)
				(*speed) = CP_GROUNDGEN_SPEED(*count);
			else if (i == UNIT_DEF_SHIELDREGEN)
				(*speed) = CP_SHIELDREGEN_SPEED(*count);
		}
		
		if ((*delay) == -1)
			if ((*count) > 0)
				(*delay) = defaults;
		if (*delay == -1)
			continue;
		tmp = (*delay);
		tmp -= (*speed) * msec * server->game.gamespeed;
		if ((tmp) < 0 && (*speed) > 0 && msec > 0)
			tmp = 0;
		if ((CP_DELAY_SEC(tmp) != CP_DELAY_SEC(*delay) || (tmp == 0 && (*delay) != 0)) && !(i != UNIT_DEF_CLUSTERBOMB && ((*count) == 0) && *delay <= 0)) {
			messageBufferPushDirect(player, player, MSG_SEND_CP_DELAY, i, 100 - (tmp) * 100 / defaults, NULL);
		}
		(*delay) = tmp;
		e->temp = 0;
	}

	return;
}


void controlpointDelayLoop(int msec, SERVER_UNIT *unit) {
	int tmp, *delay, defaults;

	switch (unit->type) {
		default:
			return;
	}

	tmp = *delay;
	if (tmp == -1)
		tmp = defaults;
	tmp -= msec * server->game.gamespeed;
	if (tmp < 0)
		tmp = 0;
	if (CP_DELAY_SEC(tmp) != CP_DELAY_SEC(*delay) || (tmp == 0 && *delay != 0))
		messageBufferPushDirect(unit->owner, unit->owner, MSG_SEND_CP_DELAY, unit->type, 100 - (tmp) * 100 / defaults, NULL);
	*delay = tmp;

	return;
}


void controlpointRadarEnd(int player) {
	int index_dst = server->player[player].cp.radar_pos;
	playerCalcLOS(player, index_dst % server->w, index_dst / server->w, -1 * unitRange(UNIT_DEF_RADAR));

	return;
}


void controlpointLoop(int msec) {
	CONTROLPOINT_EXTRA *next;
	int i;

	if (!server)
		return;

	for (next = server->controlpoint; next; next = (CONTROLPOINT_EXTRA *) next->next) {
		switch (next->type) {
			case UNIT_DEF_CLUSTERBOMB:
				if (!server->player[next->owner].map[next->index].power)
					continue;
				server->player[next->owner].cp.clusterbomb.temp++;
				break;
			case UNIT_DEF_RADAR:
				if (!server->player[next->owner].map[next->index].power)
					continue;
				server->player[next->owner].cp.radar.temp++;
				break;
			case UNIT_DEF_GROUNDGEN:
				if (!server->player[next->owner].map[next->index].power)
					continue;
				server->player[next->owner].cp.groundgen.temp++;
				break;
			case UNIT_DEF_SHIELDREGEN:
				if (!server->player[next->owner].map[next->index].power)
					continue;
				server->player[next->owner].cp.shieldregen.temp++;
				break;
			default:
				break;
		}

		/* TODO: Implement */
	}

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME_NOW)
			continue;
		controlpointDelayLoopConst(msec, i);

		if (server->player[i].cp.radar_deploy <= 0)
			continue;
		server->player[i].cp.radar_deploy -= msec * server->game.gamespeed;
		if (server->player[i].cp.radar_deploy <= 0) {
			controlpointRadarEnd(i);
			server->player[i].cp.radar_deploy = 0;
		}
	}

	return;
}


int controlpointClusterbombAlreadyUsed(int tx, int ty, int bombs) {
	int index, i;

	index = tx + ty * server->w;

	for (i = 0; i < bombs && server->clusterbomb_buffer[i] != index; i++);
	if (i == bombs)
		return 0;
	return 1;
}


void controlpointDeployClusterbomb(int player, int index_dst) {
	int bombs, x, y, tx, ty, i, radius, j, index, target, damage_deflection;

	bombs = unit_damage[UNIT_DEF_CLUSTERBOMB] / unit_maxshield[UNIT_DEF_GENERATOR];
	x = index_dst % server->w;
	y = index_dst / server->w;

	for (i = 0; i < bombs; i++) {
			do {
				tx = ty = -1;
				for (radius = 1; radius < unit_range[UNIT_DEF_CLUSTERBOMB]; radius++) {
					/* drop the bomb at this distance from ground with 7/10 chance */
					if (rand() % 10 < 7) {
						tx = rand() % (radius * 2 + 1) - radius;
						ty = rand() % (radius * 2 + 1) - radius;

						if (controlpointClusterbombAlreadyUsed(tx, ty, i)) {
							/* if this spot is occupied, just try the next size radius */
							/* will hopefully prevent lockups where all spots
							 * within a particular radius is used up */
							continue;
						} else {
							/* get out of here and see if this is within
							 * the radius */
							break;
						}
					}

				}
				/* if not dropped yet, just drop it for gods sake */
				/* will be responsible for (0.7^range * bombs) drops,
				 * plus some stray ones. */
				/* by this time we're desperate, so just do a loop which may
				 * never terminate... */
				if (tx == -1 && ty == -1) {
					do {
						tx = rand() % (radius * 2 + 1) - radius;
						ty = rand() % (radius * 2 + 1) - radius;
					} while (controlpointClusterbombAlreadyUsed(tx, ty, i));
				}

				/* this is outside of the (circular) radius and thus not a valid
				 * dropping point! */
			} while (tx*tx + ty*ty > radius*radius);

		if (!server->map[server->player[player].spawn.index])
			return;

		damage_deflection = rand() % UNIT_NUKE_MAX_DEFLECTION;
		index = tx + ty * server->w;
		server->clusterbomb_buffer[i] = index;
		tx += x;
		ty += y;
		if (tx < 0 || tx >= server->w)
			continue;
		if (ty < 0 || ty >= server->h)
			continue;
		index = tx + ty * server->w;

		target = server->map[server->player[player].spawn.index]->target;
		server->map[server->player[player].spawn.index]->target = index;

		unitDamagePoke(server->player[player].spawn.index, unit_maxshield[UNIT_DEF_GENERATOR] - damage_deflection);
		/* Another evil hack */
		if (!server->map[server->player[player].spawn.index])
			return;
		server->map[server->player[player].spawn.index]->target = target;

		for (j = 0; j < server->players; j++) {
			if (server->player[j].status != PLAYER_IN_GAME_NOW)
				continue;
			if (server->player[j].map[index].fog)
				messageBufferPushDirect(j, player, MSG_SEND_MAJOR_IMPACT, 0, index, NULL);
		}

	}

	for (i = 0; i < bombs; i++)
		server->clusterbomb_buffer[i] = ~0;

	return;
}


void controlpointDeployRadar(int player, int index_dst) {
	server->player[player].cp.radar_pos = index_dst;
	server->player[player].cp.radar_deploy = CP_RADAR_DEPLOY_TIME;

	playerCalcLOS(player, index_dst % server->w, index_dst / server->w, unitRange(UNIT_DEF_RADAR));

	return;
}


void controlpointDeployGroundgen(int player, int index_dst) {
	int i, j, range, x, y, index, k;

	x = index_dst % server->w;
	y = index_dst / server->w;

	range = unit_range[UNIT_DEF_GROUNDGEN];
	for (i = range * -1; i < range + 1; i++) {
		if (x + i < 0 || x + i >= server->w)
			continue;
		for (j = range * -1; j < range + 1; j++) {
			if (y + j < 0 || y + j >= server->h)
				continue;
			if (i*i + j*j > range*range)
				continue;
			index = (x + i) + (j + y) * server->w;
			if (!(server->map_c.tile_data[index] & 0x20000))
				continue;
			server->map_c.tile_data[index] &= 0xE0FFF;
			server->map_c.tile_data[index] |= 0x60000;
			if ((server->map_c.tile_data[index] & 0xFFF) == UNIT_BUILDSITE)
				unitAnnounce(0, player, UNIT_BUILDSITE, index);
			for (k = 0; k < server->players; k++) {
				if (server->player[i].status == PLAYER_UNUSED)
					continue;
				if (!server->player[i].map[index].fog)
					continue;
				messageBufferPushDirect(i, player, MSG_SEND_MAP_TILE_ATTRIB, 0x11, index, NULL);
			}
		}
	}

	return;
}


void controlpointDeployShieldregen(int player, int index_dst) {
	int i, j, k, range, index, x, y, deflection;
	SERVER_UNIT *unit;

	x = index_dst % server->w;
	y = index_dst / server->w;
	range = unit_range[UNIT_DEF_SHIELDREGEN];

	for (i = range * -1; i < range + 1; i++) {
		if (x + i < 0 || x + i >= server->w)
			continue;
		for (j = range * -1; j < range + 1; j++) {
			if (y + j < 0 || y + j >= server->h)
				continue;
			if (i*i + j*j > range*range)
				continue;
			index = (x + i) + (j + y) * server->w;
			if (!server->map[index])
				continue;
			if (server->map[index]->owner != player && server->player[player].team == -1)
				continue;
			if (server->player[server->map[index]->owner].team != server->player[player].team)
				continue;
			unit = server->map[index];

			deflection = rand() % UNIT_SHIELDREGEN_MAX_DEFLECTION;
			unit->shield += unit_damage[UNIT_DEF_SHIELDREGEN] - deflection;
			if (unit->shield > unit_maxshield[unit->type])
				server->map[index]->shield = unit_maxshield[unit->type];
			for (k = 0; k < server->players; k++) {
				if (server->player[k].status == PLAYER_UNUSED)
					continue;
				if (!server->player[k].map[index].fog)
					continue;
				messageBufferPushDirect(i, player, MSG_SEND_BUILDING_SHIELD, 100 * unit->shield / unit_maxshield[unit->type], index, NULL);
			}
		}
	}

	return;
}


void controlpointDeploy(int player, int type, int index_dst) {
	if (index_dst < 0 || index_dst > server->w * server->h)
		return;
	switch (type) {
		case UNIT_DEF_CLUSTERBOMB:
			if (!server->player[player].map[index_dst].fog)
				return;
			if (server->player[player].cp.clusterbomb_delay != 0)
				return;
			controlpointDeployClusterbomb(player, index_dst);
			if (server->player[player].cp.clusterbomb_delay == 0)
				server->player[player].cp.clusterbomb_delay = CP_CLUSTERBOMB_DELAY;
			break;
		case UNIT_DEF_RADAR:
			if (server->player[player].cp.radar_delay != 0)
				return;
			controlpointDeployRadar(player, index_dst);
			if (server->player[player].cp.radar_delay == 0)
				server->player[player].cp.radar_delay = CP_RADAR_DELAY;
			break;
		case UNIT_DEF_GROUNDGEN:
			if (!server->player[player].map[index_dst].fog)
				return;
			if (server->player[player].cp.groundgen_delay != 0)
				return;
			controlpointDeployGroundgen(player, index_dst);
			if (server->player[player].cp.groundgen_delay == 0)
				server->player[player].cp.groundgen_delay = CP_GROUNDGEN_DELAY;
			break;
		case UNIT_DEF_SHIELDREGEN:
			if (!server->player[player].map[index_dst].fog)
				return;
			if (server->player[player].cp.shieldregen_delay != 0)
				return;
			controlpointDeployShieldregen(player, index_dst);
			if (server->player[player].cp.shieldregen_delay == 0)
				server->player[player].cp.shieldregen_delay = CP_SHIELDREGEN_DELAY;
			break;
		default:
			return;
			break;
	}

	controlpointLoop(0);

	messageBufferPushDirect(player, player, MSG_SEND_CP_DEPLOY, type, index_dst, NULL);
	if (type != UNIT_DEF_CLUSTERBOMB || (server->player[player].cp.clusterbomb.count > 0 || server->player[player].cp.clusterbomb_delay > 0))
		messageBufferPushDirect(player, player, MSG_SEND_CP_DELAY, type, 0, NULL);

	return;
}

