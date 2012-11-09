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

#include <string.h>

#include "muon.h"
#include "view.h"
#include "game.h"
//#include "client.h"
//#include "engine.h"
#include "chat.h"
#include "menu.h"
#include "lobby.h"
#include "map.h"

void view_init() {
	//TODO: lots of breaking out to separate functions, game menu and lobby for example
	font_std=darnitFontLoad("../res/FreeMonoBold.ttf", 12, 512, 512);
	mouse_tilesheet=darnitRenderTilesheetLoad("../res/mouse.png", 16, 16, DARNIT_PFORMAT_RGB5A1);
	
	menu_init();
	lobby_init();
	game_view_init();
	
	gamestate_pane[0]=&panelist_menu_sidebar;
	gamestate_pane[1]=&panelist_connecting;
	gamestate_pane[2]=&panelist_lobby;
	gamestate_pane[3]=&panelist_game_sidebar;
	gamestate_pane[4]=&panelist_game_menu;
	gamestate_pane[5]=NULL;
	
	map=NULL;
}

void view_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	float r, g, b, a;
	darnitRenderTintGet(&r, &g, &b, &a);
	darnitRenderTint(1, 1, 1, 1);
	darnitRenderBlendingEnable();
	darnitRenderTileBlit(mouse_tilesheet, 0, e->mouse->x, e->mouse->y);
	darnitRenderBlendingDisable();
	darnitRenderTint(r, g, b, a);
}
