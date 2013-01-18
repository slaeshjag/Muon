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


int controlpointBuildsiteInit(SERVER_UNIT *unit) {
	server->player[unit->owner].buildspots++;
	server->player[unit->owner].buildspeed = logf(M_E + server->player[unit->owner].buildspots);
	unit->cp = NULL;

	return 0;
}


void controlpointInitPlayer(int player) {
	server->player[player].cp.clusterbomb_delay = -1;
	server->player[player].cp.radar_delay = -1;
	server->player[player].cp.radar_deploy = -1;
	server->player[player].cp.radar_pos = -1;

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
		default:
			return 0;
	}

	/* Stub for now */

	return 0;
}


void controlpointBuildsiteRemove(SERVER_UNIT *unit) {
	server->player[unit->owner].buildspots--;
	server->player[unit->owner].buildspeed = logf(M_E + server->player[unit->owner].buildspots);

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
		default:
			return;
	}

	for (next = server->controlpoint; next; next = (void *) next->next)
		if (next->owner == unit->owner)
			if (next->type == unit->type)
				if (unit->cp != next)
					return;
	*val = 0;
	messageBufferPushDirect(unit->owner, unit->owner, MSG_SEND_CP_CLEAR, unit->type, 0, NULL);

	return;
}


void controlpointClusterbombRemove(SERVER_UNIT *unit) {
	controlpointDelayReset(unit);

	return;
}


void controlpointRadarRemove(SERVER_UNIT *unit) {
	controlpointDelayReset(unit);

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


void controlpointDelayLoop(int msec, SERVER_UNIT *unit) {
	int tmp, *delay, defaults;

	switch (unit->type) {
		case UNIT_DEF_CLUSTERBOMB:
			delay = &server->player[unit->owner].cp.clusterbomb_delay;
			defaults = CP_CLUSTERBOMB_DELAY;
			break;
		case UNIT_DEF_RADAR:
			delay = &server->player[unit->owner].cp.radar_delay;
			defaults = CP_RADAR_DELAY;
			break;
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
	playerCalcLOS(player, index_dst % server->w, index_dst / server->w, -1);

	return;
}


void controlpointLoop(int msec) {
	CONTROLPOINT_EXTRA *next;
	SERVER_UNIT *unit;
	int i;

	if (!server)
		return;
	
	for (next = server->controlpoint; next; next = (CONTROLPOINT_EXTRA *) next->next) {
		unit = server->map[next->index];
		switch (next->type) {
			case UNIT_DEF_CLUSTERBOMB:
				if (!server->player[next->owner].map[next->index].power)
					continue;
				controlpointDelayLoop(msec, server->map[next->index]);
				break;
			case UNIT_DEF_RADAR:
				if (!server->player[next->owner].map[next->index].power)
					continue;
				controlpointDelayLoop(msec, server->map[next->index]);
				break;
			default:
				break;
		}

		/* TODO: Implement */
	}
		
	for (i = 0; i < server->players; i++) {
		if (server->player[i].cp.radar_deploy <= 0)
			continue;
		server->player[unit->owner].cp.radar_deploy -= msec * server->game.gamespeed;
		if (server->player[unit->owner].cp.radar_deploy <= 0) {
			controlpointRadarEnd(unit->owner);
			server->player[unit->owner].cp.radar_deploy = 0;
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
	int bombs, x, y, tx, ty, i, j, index, target;

	bombs = unit_damage[UNIT_DEF_CLUSTERBOMB] / unit_maxshield[UNIT_DEF_GENERATOR];
	x = index_dst % server->w;
	y = index_dst / server->w;

	for (i = 0; i < bombs; i++) {
		do {
			tx = rand() % (unit_range[UNIT_DEF_CLUSTERBOMB] * 2 + 1) - unit_range[UNIT_DEF_CLUSTERBOMB];
			ty = rand() % (unit_range[UNIT_DEF_CLUSTERBOMB] * 2 + 1) - unit_range[UNIT_DEF_CLUSTERBOMB];
		} while (controlpointClusterbombAlreadyUsed(tx, ty, i));
		
		if (!server->map[server->player[player].spawn.index])
			return;

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

		unitDamagePoke(server->player[player].spawn.index, unit_maxshield[UNIT_DEF_GENERATOR]);
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
	
	playerCalcLOS(player, index_dst % server->w, index_dst / server->w, 1);

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
			if (server->player[player].cp.radar_delay > 0 || server->player[player].cp.radar_delay == -1)
				return;
			controlpointDeployRadar(player, index_dst);
			if (server->player[player].cp.radar_delay == 0)
				server->player[player].cp.radar_delay = CP_RADAR_DELAY;
			break;
		default:
			return;
			break;
	}

	messageBufferPushDirect(player, player, MSG_SEND_CP_DEPLOY, type, index_dst, NULL);

	return;
}

