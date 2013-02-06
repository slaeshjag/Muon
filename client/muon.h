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

#ifndef MUON_H
#define MUON_H

#include <stdio.h>
#include <stdlib.h>

#include <darnit/darnit.h>
#include "../server/server_api/server.h"

#include "platform.h"
#include "ui/ui.h"
#include "view.h"

char *const *args;

typedef enum {
	GAME_STATE_MENU,
	GAME_STATE_CREDITS,
	GAME_STATE_CONNECTING,
	GAME_STATE_GAME_OVER,
	GAME_STATE_LOBBY,
	GAME_STATE_GAME,
	GAME_STATE_GAME_MENU,
	GAME_STATE_QUIT,
	
	GAME_STATES,
} GAME_STATE;

GAME_STATE gamestate;
struct UI_PANE_LIST *gamestate_pane[GAME_STATES];

UI_EVENT_BUTTONS prevbuttons;

void game_state(GAME_STATE state);

#endif
