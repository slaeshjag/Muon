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
	new->delay = CP_CLUSTERBOMB_DELAY;
	new->index = unit->x + unit->y * server->w;
	new->owner = unit->owner;

	return 0;
}


int controlpointBuildsiteInit(SERVER_UNIT *unit) {
	server->player[unit->owner].buildspots++;
	server->player[unit->owner].buildspeed = logf(M_E + server->player[unit->owner].buildspots);

	return 0;
}


int controlpointInit() {
	server->controlpoint = NULL;

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

	for (next = server->controlpoint; next; next = (CONTROLPOINT_EXTRA *) next->next) {
		switch (next->type) {
			case UNIT_DEF_CLUSTERBOMB:
				next->delay -= msec * server->game.gamespeed;
				if (next->delay < 0)
					next->delay = 0;
		}

		/* TODO: Implement */
	}

	return;
}
