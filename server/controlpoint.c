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


int controlpointClusterbombInit(SERVER_UNIT *unit) {
	CONTROLPOINT_EXTRA *new;

	if (!(new = malloc(sizeof(CONTROLPOINT_EXTRA))))
		return -1;
	new->next = (struct CONTROLPOINT_EXTRA *) server->controlpoint;
	server->controlpoint = new;

	new->type = unit->type;
	new->index = unit->x + unit->y * server->w;
	new->owner = unit->owner;

	return 0;
}


int controlpointBuildsiteInit(SERVER_UNIT *unit) {
	server->player[unit->owner].buildspots++;
	server->player[unit->owner].buildspeed = logf(M_E + server->player[unit->owner].buildspots);

	return 0;
}


void controlpointInitPlayer(int player) {
	server->player[player].cp.clusterbomb_delay = -1;
	server->player[player].cp.radar_delay = -1;

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


void controlpointClusterbombRemove(SERVER_UNIT *unit) {
	return;
}


void controlpointRemove(struct SERVER_UNIT *unit) {
	CONTROLPOINT_EXTRA *next;
	switch (unit->type) {
		case UNIT_DEF_BUILDSITE:
			controlpointBuildsiteRemove(unit);
			return;
		case UNIT_DEF_CLUSTERBOMB:
			controlpointClusterbombRemove(unit);
			return;
		default:
			break;
	}

	if (!server->controlpoint) {
		fprintf(stderr, "Internal error: Control point free'd but not in list\n");
		free(unit->cp);
		return;
	}

	for (next = server->controlpoint; next->next && (void *) next->next != unit->cp; next = (void *) next->next);

	if (next->next != (void *) unit->cp) {
		fprintf(stderr, "Internal error: Control point free'd but not in list\n");
		free(unit->cp);
		return;
	}

	next->next = ((CONTROLPOINT_EXTRA *)((CONTROLPOINT_EXTRA *)next)->next)->next;
	free(unit->cp);

	return;
}


void controlpointLoop(int msec) {
	CONTROLPOINT_EXTRA *next;
	int delay;

	if (!server)
		return;
	
	for (next = server->controlpoint; next; next = (CONTROLPOINT_EXTRA *) next->next) {
		switch (next->type) {
			case UNIT_DEF_CLUSTERBOMB:
				delay = server->player[next->owner].cp.clusterbomb_delay;
				if (delay == -1)
					delay = CP_CLUSTERBOMB_DELAY;
				delay -= msec * server->game.gamespeed;
				if (delay < 0)
					delay = 0;
				if (CP_DELAY_SEC(delay) != CP_DELAY_SEC(server->player[next->owner].cp.clusterbomb_delay))
					messageBufferPushDirect(next->owner, next->owner, MSG_SEND_CP_DELAY, UNIT_DEF_CLUSTERBOMB, CP_DELAY_SEC(delay), NULL);
				server->player[next->owner].cp.clusterbomb_delay = delay;
				break;
			case UNIT_DEF_RADAR:
				delay = server->player[next->owner].cp.radar_delay;
				if (delay == -1)
					delay = CP_RADAR_DELAY;
				delay -= msec * server->game.gamespeed;
				if (delay < 0)
					delay = 0;
				if (CP_DELAY_SEC(delay) != CP_DELAY_SEC(server->player[next->owner].cp.radar_delay))
					messageBufferPushDirect(next->owner, next->owner, MSG_SEND_CP_DELAY, UNIT_DEF_RADAR, CP_DELAY_SEC(delay), NULL);
				server->player[next->owner].cp.radar_delay = delay;
				break;
			default:
				break;
		}

		/* TODO: Implement */
	}

	return;
}


int controlpointClusterbombAlreadyUsed(int tx, int ty, int bombs) {
	int index, i;

	index = tx + ty * server->w;
	
	for (i = 0; i < bombs && server->clusterbomb_buffer[i] != index; i++);
	if (i == bombs)
		return 1;
	return 0;
}


void controlpointDeployClusterbomb(int player, int index_dst) {
	int bombs, x, y, tx, ty, i, index;

	bombs = unit_damage[UNIT_DEF_CLUSTERBOMB] / unit_maxshield[UNIT_DEF_GENERATOR];
	x = index_dst % server->w;
	y = index_dst / server->w;

	for (i = 0; i < bombs; i++) {
		do {
			tx = rand() % (unit_range[UNIT_DEF_CLUSTERBOMB] * 2 + 1) - unit_range[UNIT_DEF_CLUSTERBOMB];
			ty = rand() % (unit_range[UNIT_DEF_CLUSTERBOMB] * 2 + 1) - unit_range[UNIT_DEF_CLUSTERBOMB];
		} while (controlpointClusterbombAlreadyUsed(tx, ty, i));
		
		index = tx + ty * server->w;
		server->clusterbomb_buffer[i] = index;
		tx += x;
		ty += y;
		index = tx + ty * server->w;
		unitDamagePoke(index, unit_damage[UNIT_DEF_CLUSTERBOMB]);
	}

	for (i = 0; i < bombs; i++)
		server->clusterbomb_buffer[i] = ~0;

	return;
}


void controlpointDeployRadar(int player, int index_dst) {
	return;
}


void controlpointDeploy(int player, int type, int index_dst) {
	
	if (index_dst < 0 || index_dst > server->w * server->h)
		return;
	switch (type) {
		case UNIT_DEF_CLUSTERBOMB:
			if (server->player[player].cp.clusterbomb_delay <= 0)
				return;
			controlpointDeployClusterbomb(player, index_dst);
			break;
		case UNIT_DEF_RADAR:
			if (server->player[player].cp.radar_delay <= 0)
				return;
			controlpointDeployRadar(player, index_dst);
			break;
		default:
			return;
			break;
	}

	messageBufferPushDirect(player, player, MSG_SEND_CP_DEPLOY, type, index_dst, NULL);

	return;
}
				
