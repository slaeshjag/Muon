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
#include "menu/credits.h"
#include "view.h"
#include "client.h"
#include "chat.h"
#include "game.h"
#include "gameover.h"
#include "intmath.h"

void (*state_render[])()={
		view_background_draw,
		credits_draw,
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
		case GAME_STATE_CREDITS:
			//d_render_fade_out(1000);
		case GAME_STATE_GAME_OVER:
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
		case GAME_STATE_CREDITS:
			d_render_fade_in(500, 0, 0, 0);
			ui_event_global_add(menu_buttons, UI_EVENT_TYPE_BUTTONS);
			break;
		case GAME_STATE_GAME_OVER:
			gameover_update_stats();
			client_disconnect(MSG_RECV_GAME_ENDED);
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

void sound_init() {
	sound_ready=d_sound_streamed_load("res/ready.ogg", 1, 0);
	sound_defeated=d_sound_streamed_load("res/defeated.ogg", 1, 0);
	sound_explosion=d_sound_streamed_load("res/explosion.ogg", 1, 0);
	sound_chat=d_sound_streamed_load("res/chat.ogg", 1, 0);
	sound_flare=d_sound_streamed_load("res/flare.ogg", 1, 0);
	sound_nuke=d_sound_streamed_load("res/nuke.ogg", 1, 0);
	sound_radar=d_sound_streamed_load("res/radar.ogg", 1, 0);
}

void sound_play(DARNIT_SOUND *snd) {
	if(snd)
		d_sound_play(snd, 0, config.sound*12, config.sound*12, 0);
}

int main(int argc, char **argv) {
	args=argv;
	platform_init("Muon", "res/icon.png");
	
	player_id=0;
	memset(&prevbuttons, 0, sizeof(prevbuttons));
	server_init();
	
	intmath_init();
	ui_init(platform_lowres?2:UI_PADDING_DEFAULT);
	view_init();
	sound_init();
	
	ui_event_global_add(view_mouse_draw, UI_EVENT_TYPE_UI);
	game_state(GAME_STATE_MENU);
	
	while(gamestate!=GAME_STATE_QUIT) {
		server_loop(d_last_frame_time());
		
		if(gamestate>=GAME_STATE_LOBBY)
			client_check_incoming();
		
		d_render_begin();
		if(state_render[gamestate])
			state_render[gamestate]();
		
		d_render_tint(player_color[player_id].r, player_color[player_id].g, player_color[player_id].b, 255);
		if(gamestate==GAME_STATE_GAME_MENU) {
			ui_pane_render(panelist_game_sidebar.pane);
			ui_pane_render(panelist_game_abilitybar.pane);
		}
		ui_events(gamestate_pane[gamestate], 1);
		d_render_tint(255, 255, 255, 255);
		
		d_render_end();
		d_loop();
	}
	
	if(server_is_running())
		server_stop();
	
	platform_config_write();
	d_quit();	
	return 0;
}
