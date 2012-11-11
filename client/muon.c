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
#include "client.h"
#include "chat.h"
#include "game.h"
#include "intmath.h"
#include "menu.h"

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
	if(gamestate==GAME_STATE_GAME) {
		chat_indicator_hide(&panelist_game_sidebar);
		ui_event_global_remove(game_view_buttons, UI_EVENT_TYPE_BUTTONS);
		ui_event_global_remove(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_PRESS);
		ui_event_global_remove(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
		ui_event_global_remove(game_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
		ui_event_global_remove(game_view_key_press, UI_EVENT_TYPE_KEYBOARD_PRESS);
	}
	//Game state constructors
	switch(state) {
		/*case GAME_STATE_INPUT_NAME:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			ui_selected_widget=input_name_entry;
			break;*/
		case GAME_STATE_LOBBY:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			chat_show(gamestate_pane[GAME_STATE_LOBBY]);
			ui_selected_widget=chat_entry;
			break;
		case GAME_STATE_GAME:
			darnitRenderClearColorSet(0x7f, 0x7f, 0x7f);
			ui_event_global_add(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_PRESS);
			ui_event_global_add(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_view_buttons, UI_EVENT_TYPE_BUTTONS);
			ui_event_global_add(game_view_key_press, UI_EVENT_TYPE_KEYBOARD_PRESS);
			//darnitInputGrab();
		case GAME_STATE_CONNECTING:
			ui_selected_widget=NULL;
			break;
		/*case GAME_STATE_CONNECT_SERVER:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			ui_selected_widget=connect_server_entry_host;*/
		case GAME_STATE_MENU:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
		case GAME_STATE_QUIT:
		case GAME_STATE_GAME_MENU:
			darnitInputUngrab();
			break;
	}
	
	gamestate=state;
}

int main() {
	platform_init();
	
	player_id=0;
	serverInit();
	serverStart("../res/Twirly 0.1 (2).ldi", 2, 1337, 3);
	
	intmath_init();
	ui_init();
	view_init();
	chat_init();
	
	ui_event_global_add(view_mouse_draw, UI_EVENT_TYPE_UI);
	game_state(GAME_STATE_MENU);
	
	while(gamestate!=GAME_STATE_QUIT) {
		serverLoop(darnitTimeLastFrameTook());
		
		if(gamestate>=GAME_STATE_LOBBY)
			client_check_incomming();
		
		darnitRenderBegin();
		if(state_render[gamestate])
			state_render[gamestate]();
			
		darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
		if(gamestate==GAME_STATE_GAME_MENU)
			ui_pane_render(panelist_game_sidebar.pane);
		ui_events(gamestate_pane[gamestate], 1);
		darnitRenderTint(1, 1, 1, 1);
		
		darnitRenderEnd();
		
		darnitLoop();
	}
	
	return 0;
}
