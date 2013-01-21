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
#include "view.h"
#include "client.h"
#include "chat.h"
#include "game.h"
#include "intmath.h"

void (*state_render[])()={
		view_background_draw,
		view_background_draw,
		view_background_draw,
		game_view_draw,
		game_view_draw,
		NULL,
};

void game_state(GAME_STATE state) {
	//Game state destructors
	switch(gamestate) {
		case GAME_STATE_GAME:
			chat_indicator_hide(&panelist_game_sidebar);
			ui_event_global_remove(game_view_buttons, UI_EVENT_TYPE_BUTTONS);
			ui_event_global_remove(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_PRESS);
			ui_event_global_remove(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_remove(game_mouse_draw, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_remove(game_view_key_press, UI_EVENT_TYPE_KEYBOARD_PRESS);
			break;
		case GAME_STATE_GAME_MENU:
		case GAME_STATE_MENU:
			ui_event_global_remove(menu_buttons, UI_EVENT_TYPE_BUTTONS);
		case GAME_STATE_CONNECTING:
		case GAME_STATE_LOBBY:
		case GAME_STATE_QUIT:
		
		case GAME_STATES:
			break;
	}
	//Game state constructors
	switch(state) {
		/*case GAME_STATE_INPUT_NAME:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			ui_selected_widget=input_name_entry;
			break;*/
		case GAME_STATE_GAME:
			d_render_clearcolor_set(0x7f, 0x7f, 0x7f);
			ui_event_global_add(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_PRESS);
			ui_event_global_add(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_mouse_draw, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_view_buttons, UI_EVENT_TYPE_BUTTONS);
			ui_event_global_add(game_view_key_press, UI_EVENT_TYPE_KEYBOARD_PRESS);
			#ifndef __DEBUG__
			d_input_grab();
			#endif
		case GAME_STATE_CONNECTING:
			ui_selected_widget=NULL;
			break;
		case GAME_STATE_MENU:
			d_render_clearcolor_set(0x0, 0x0, 0x0);
		case GAME_STATE_GAME_MENU:
			ui_event_global_add(menu_buttons, UI_EVENT_TYPE_BUTTONS);
		case GAME_STATE_LOBBY:
		case GAME_STATE_QUIT:
			d_input_release();
		
		case GAME_STATES:
			break;
	}
	
	gamestate=state;
}

int main() {
	platform_init();
	
	player_id=0;
	memset(&prevbuttons, 0, sizeof(prevbuttons));
	serverInit();
	
	intmath_init();
	ui_init();
	view_init();
	chat_init();
	
	ui_event_global_add(view_mouse_draw, UI_EVENT_TYPE_UI);
	game_state(GAME_STATE_MENU);
	
	while(gamestate!=GAME_STATE_QUIT) {
		serverLoop(d_last_frame_time());
		
		if(gamestate>=GAME_STATE_LOBBY)
			client_check_incoming();
		
		d_render_begin();
		if(state_render[gamestate])
			state_render[gamestate]();
		
		d_render_tint(255*(!(player_id%3)), 255*(player_id>1), 255*(player_id==1), 255);
		if(gamestate==GAME_STATE_GAME_MENU)
			ui_pane_render(panelist_game_sidebar.pane);
		ui_events(gamestate_pane[gamestate], 1);
		d_render_tint(255, 255, 255, 255);
		
		d_render_end();
		d_loop();
	}
	
	if(serverIsRunning())
		serverStop();
	
	platform_config_write();
	d_quit();	
	return 0;
}
