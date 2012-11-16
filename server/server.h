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

#ifndef __SERVER_H__
#define	__SERVER_H__

#define	SERVER_PORT			56789
#define	SERVER_VERSION			0x10000

#define	SERVER_PROCESS_NOTHING		0
#define	SERVER_PROCESS_MSG		1
#define	SERVER_PROCESS_DATA		2

#define	SERVER_PROCESS_DONE		1
#define	SERVER_PROCESS_INCOMPLETE	0
#define	SERVER_PROCESS_FAIL		-1

#define	SERVER_PING_INTERVAL		2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "network.h"
#include "superweapon.h"
#include "unit.h"
#include "message.h"
#include "message_handlers.h"
#include "player.h"
#include "game.h"
#include "lobby.h"
#include "error.h"
#ifndef __TTMZ_H__
#include "map.h"
#endif


typedef struct SERVER_UNIT {
	unsigned int		x;
	unsigned int		y;
	unsigned int		owner;
	unsigned int		type;
	int			hp;
	int			shield;
	unsigned int		status;
	unsigned int		last_no_shield;
	int			target;
	UNIT_PYLON		pylon;
	struct SERVER_UNIT	*next;
} SERVER_UNIT;


typedef struct {
	const char		*path;
	void			*data;
	unsigned int		data_len;
	unsigned int		*tile_data;
} SERVER_MAP;


typedef struct {
	SERVER_UNIT		*unit;
	MESSAGE_HANDLER		message_handler;
	unsigned int		w;
	unsigned int		h;
	SERVER_UNIT		**map;
	unsigned int		*power_grid;
	UNIT_DEF		*unit_def;
	PLAYER			*player;
	unsigned int		players;
	GAME			game;
	SERVER_SOCKET		*accept;
	void			*map_data;
	SERVER_MAP		map_c;
	UNIT_PYLON		*pylons;
	int			server_admin;
} SERVER;

SERVER *server;


int serverPowerGet(int owner, int x, int y);
SERVER *serverStart(const char *fname, unsigned int players, int port, int gamespeed);
SERVER *serverStop();
int serverLoop(unsigned int d_ms);
int serverPlayersPlaying();

#endif
