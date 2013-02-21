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

#ifndef MAPED_H
#define MAPED_H

#include <stdio.h>
#include <stdlib.h>

#include <darnit/darnit.h>

#include "../client/ui/ui.h"
#include "../client/platform.h"

#define SIDEBAR_WIDTH 128
#define MAX_PLAYERS 4
#define BUILDINGS 8

static const char mapdir[]="maps";
static const unsigned char ldimagic[]={0x83, 0xB3, 0x66, 0x1B, 0xBB, 0xA7, 0x7A, 0xBC};

enum STATE {
	STATE_MENU,
	STATE_LOAD,
	STATE_EDITOR,
	STATE_QUIT,
	STATES,
};

struct {
	void (*render)();
	struct UI_PANE_LIST *panelist;
} state[STATES];

enum STATE state_current;

DARNIT_TILESHEET *mouse_tilesheet;
DARNIT_FONT *font_std;

void state_set(enum STATE state);
void view_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
