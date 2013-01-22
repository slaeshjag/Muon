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

#define	API_VERSION			0x10000
#define	SERVER_PORT_DEFAULT		56789

#include "building.h"
#include "message.h"
#include "error.h"

typedef void SERVER;

void server_init();
SERVER *server_start(const char *map, unsigned int players, int port, int gamespeed);
void server_loop(unsigned int msec);
void server_stop();
int server_is_running();
int server_players_playing();
void server_admin_set(int player);


#endif
