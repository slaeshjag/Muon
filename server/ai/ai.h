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

#ifndef AI_H
#define AI_H

#include "../server_api/server.h"

enum PERSONALITY {
	PERSONALITY_SCOUT,
	PERSONALITIES,
};

struct BUILDING {
	int range;
} building[BUILDINGS_DEFINED];

struct UNIT {
	int x;
	int y;
	int type;
	int owner;
	struct UNIT *next;
};

struct PLAYER {
	struct {
		int x;
		int y;
	} home;
};

struct AREA {
	int x;
	int y;
	int w;
	int h;
	int value;
	int owner;
	int threat;
};

typedef struct AI {
	int player_id;
	char player_name[32];
	enum PERSONALITY personality;
	struct UNIT unit;
	int constructing;
	struct {
		int x;
		int y;
		int direction;
	} expand;
	struct PLAYER *player;
	int players;
} AI;

struct {
	struct {
		void (*idle)(AI *ai);
		void (*spot)(AI *ai);
		void (*engage)();
	} task;
} personality[PERSONALITIES];

#endif