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

#ifndef __ERROR_H__
#define	__ERROR_H__

#define	SERVER_ERROR_CANT_OPEN_MAP		1
#define	SERVER_ERROR_NO_MEMORY			2
#define	SERVER_ERROR_SPAWNS_OVERLAP		3
#define	SERVER_ERROR_INVALID_MAP		4
#define	SERVER_ERROR_MAP_NO_MAX_PLAYERS		5
#define	SERVER_ERROR_UNABLE_TO_LISTEN		6
#define	SERVER_ERROR_TOO_MANY_PLAYERS		7
#define	SERVER_ERROR_GAMESPEED_TOO_SMALL	8
#define	SERVER_ERROR_SPAWN_MISSING		9
#define	SERVER_ERROR_GENERATOR_COUNT_MISMATCH	10
#define	SERVER_ERROR_SHUTTING_DOWN		11
#define	SERVER_ERROR_ALREADY_RUNNING		12
#define	SERVER_ERROR_BAD_LUCK			13


#ifdef WITH_SERVER_ERROR

#define	SERVER_ERROR_MAX_POS	8

const char *server_errors[] = { NULL, 
				"Unable to open the map file for caching.",
				"Unable to allocate more memory. Your system might've runned out of RAM.",
				"The map contains overlapping spawn points. This is not acceptable.",
				"The map file is invalid.",
				"The map is missing the property 'max_players'.",
				"The server was unable to bind the requested port. It's either in use or blocked by a firewall.",
				"The map is not defined for this many players.",
				"Gamespeed must be bigger than 0.",
				"The map is missing a generator for one or more players.",
				"The number of generators on the map is not equal to the biggest player id.",
				"The server is still sending out data to clients and has not yet stopped.",
				"The server is already running and has no intention of stopping on its own.",
				"rand() wasn't on our side, and spawn randomization took too many tries. Try again."};

typedef struct {
	int			read_pos;
	int			write_pos;
	int			err[SERVER_ERROR_MAX_POS];
} SERVER_ERROR;


SERVER_ERROR server_error;

#endif

int errorInit();
void errorPush(int error);
const char *errorPop();


#endif
