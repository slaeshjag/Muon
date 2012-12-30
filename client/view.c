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
#include "menu/menu.h"
#include "menu/multiplayer.h"
#include "view.h"
#include "game.h"
#include "chat.h"
#include "lobby.h"
#include "intmath.h"
#include "map.h"

void view_init() {
	font_std=darnitFontLoad("res/FreeMonoBold.ttf", 12, 512, 512);
	mouse_tilesheet=darnitRenderTilesheetLoad("res/mouse.png", 16, 16, DARNIT_PFORMAT_RGB5A1);
	
	//Menu background
	int scale=config.plasma==0?1:(1<<(5-config.plasma));
	view_background_w=platform.screen_w/scale;
	view_background_h=platform.screen_h/scale;
	view_background_ts=darnitRenderTilesheetNew(1, 1, view_background_w, view_background_h, DARNIT_PFORMAT_RGB5A1);
	view_background_tile=darnitRenderTileAlloc(1);
	view_background_pixbuf=malloc(view_background_w*view_background_h*sizeof(unsigned int));
	darnitRenderTilesheetScaleAlgorithm(view_background_ts, DARNIT_SCALE_LINEAR);
	darnitRenderTileMove(view_background_tile, 0, view_background_ts, 0, 0);
	darnitRenderTileSetTilesheetCoord(view_background_tile, 0, view_background_ts, 0, 0, view_background_w, view_background_h);
	darnitRenderTileSizeSet(view_background_tile, 0, platform.screen_w, platform.screen_h);
	view_background_update(0);
	
	menu_init();
	lobby_init();
	game_view_init();
	
	gamestate_pane[0]=&panelist_menu_sidebar;
	gamestate_pane[1]=&panelist_multiplayer_connecting;
	gamestate_pane[2]=&panelist_lobby_players;
	gamestate_pane[3]=&panelist_game_sidebar;
	gamestate_pane[4]=&panelist_game_menu;
	gamestate_pane[5]=NULL;
	
	map=NULL;
}

void view_background_update(int t) {
	static int x, y, mov1, mov2, c1, c2, c3;
	for (y=0; y<view_background_h; y++)
		for (x=0; x<view_background_w; x++) {
			mov1=360*y/view_background_h+(t>>1);
			mov2=360*x/view_background_w;
			c1=sine(mov1+(t>>1))/2+((mov2>>1)-mov1-mov2+(t>>1));
			c2=sine((c1+sine((y>>2)+(t>>1))+sine((x+y)))/10);
			c3=sine((c2+(cosine(mov1+mov2+c2/10)>>2)+cosine(mov2)+sine(x))/10);
			view_background_pixbuf[y*view_background_w+x]=(c1+c2+c3)/360+26;
		}
	darnitRenderTilesheetUpdate(view_background_ts, 0, 0, view_background_w, view_background_h, view_background_pixbuf);
}

void view_background_draw() {
	static int t=0, recalc=0;
	if(config.plasma&&recalc) {
		view_background_update(t);
		t++;
	}
	recalc=!recalc;
	
	darnitRenderTileDraw(view_background_tile, view_background_ts, 1);
}

void view_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	float r, g, b, a;
	darnitRenderTintGet(&r, &g, &b, &a);
	darnitRenderTint(1, 1, 1, 1);
	darnitRenderBlendingEnable();
	darnitRenderTileBlit(mouse_tilesheet, 0, e->mouse->x, e->mouse->y);
	darnitRenderBlendingDisable();
	darnitRenderTint(r, g, b, a);
}
