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
#include "gameover.h"
#include "chat.h"
#include "lobby.h"
#include "intmath.h"
#include "map.h"

void view_init() {
	font_std=d_font_load("res/FreeMonoBold.ttf", 12, 512, 512);
	mouse_tilesheet=d_render_tilesheet_load("res/mouse.png", 16, 16, DARNIT_PFORMAT_RGB5A1);
	mouse_target_tilesheet=d_render_tilesheet_load("res/target.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	
	//Menu background
	int scale=config.plasma==0?1:(1<<(5-config.plasma));
	view_background_w=platform.screen_w/scale;
	view_background_h=platform.screen_h/scale;
	view_background_ts=d_render_tilesheet_new(1, 1, view_background_w, view_background_h, DARNIT_PFORMAT_RGB5A1);
	view_background_tile=d_render_tile_new(1, view_background_ts);
	view_background_pixbuf=malloc(view_background_w*view_background_h*sizeof(unsigned int));
	d_render_tilesheet_scale_algorithm(view_background_ts, DARNIT_SCALE_LINEAR);
	d_render_tile_move(view_background_tile, 0, 0, 0);
	d_render_tile_tilesheet_coord_set(view_background_tile, 0, 0, 0, view_background_w, view_background_h);
	d_render_tile_size_set(view_background_tile, 0, platform.screen_w, platform.screen_h);
	view_background_update(0);
	
	menu_init();
	lobby_init();
	game_view_init();
	gameover_init();
	chat_init();
	
	gamestate_pane[GAME_STATE_MENU]=&panelist_menu_sidebar;
	gamestate_pane[GAME_STATE_CONNECTING]=&panelist_multiplayer_connecting;
	gamestate_pane[GAME_STATE_LOBBY]=&panelist_lobby_players;
	gamestate_pane[GAME_STATE_GAME]=&panelist_game_sidebar;
	gamestate_pane[GAME_STATE_GAME_MENU]=&panelist_game_menu;
	gamestate_pane[GAME_STATE_GAME_OVER]=&panelist_gameover_sidebar;
	gamestate_pane[GAME_STATE_QUIT]=NULL;
	
	map=NULL;
}

void view_background_update(int t) {
	static int x, y, mov1, mov2, c1, c2, c3;
	for (y=0; y<view_background_h; y++)
		for (x=0; x<view_background_w; x++) {
			mov1=(y<<8)/view_background_h+(t>>1);
			mov2=(x<<8)/view_background_w;
			c1=sine(mov1+(t>>1))/2+((mov2>>1)-mov1-mov2+(t>>1));
			c2=sine((c1+sine((y>>2)+(t>>1))+sine((x+y)))/8);
			c3=sine((c2+(cosine(mov1+mov2+c2/8)>>2)+cosine(mov2)+sine(x))/8);
			view_background_pixbuf[y*view_background_w+x]=(c1+c2+c3)/256+32;
		}
	d_render_tilesheet_update(view_background_ts, 0, 0, view_background_w, view_background_h, view_background_pixbuf);
}

void view_background_draw() {
	static int t=0, recalc=0;
	if(config.plasma&&recalc) {
		view_background_update(t);
		t++;
	}
	recalc=!recalc;
	
	d_render_tile_draw(view_background_tile, 1);
}

void view_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(255, 255, 255, 255);
	d_render_blend_enable();
	d_render_tile_blit(mouse_tilesheet, 0, e->mouse->x, e->mouse->y);
	d_render_blend_disable();
	d_render_tint(r, g, b, a);
}
