#include <string.h>

#include "muon.h"
#include "view.h"
#include "client.h"
#include "chat.h"
#include "game.h"
#include "menu.h"

void game_state(GAME_STATE state) {
	//Game state destructors
	if(gamestate==GAME_STATE_GAME) {
		ui_event_global_remove(game_view_buttons, UI_EVENT_TYPE_BUTTONS);
		ui_event_global_remove(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_DOWN);
		ui_event_global_remove(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
		ui_event_global_remove(game_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
		ui_event_global_remove(game_view_key_press, UI_EVENT_TYPE_KEYBOARD_PRESS);
	}
	//Game state constructors
	switch(state) {
		case GAME_STATE_INPUT_NAME:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			ui_selected_widget=input_name_entry;
			break;
		case GAME_STATE_LOBBY:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			chat_show(gamestate_pane[GAME_STATE_LOBBY]);
			ui_selected_widget=chat_entry;
			break;
		case GAME_STATE_GAME:
			darnitRenderClearColorSet(0x7f, 0x7f, 0x7f);
			ui_event_global_add(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_DOWN);
			ui_event_global_add(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_view_buttons, UI_EVENT_TYPE_BUTTONS);
			ui_event_global_add(game_view_key_press, UI_EVENT_TYPE_KEYBOARD_PRESS);
			//darnitInputGrab();
		case GAME_STATE_CONNECTING:
			ui_selected_widget=NULL;
			break;
		case GAME_STATE_CONNECT_SERVER:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			ui_selected_widget=connect_server_entry_host;
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
	serverStart("../res/map.ldi", 2, 1337, 10);
	
	ui_init();
	view_init();
	chat_init();
	
	ui_event_global_add(view_draw_mouse, UI_EVENT_TYPE_UI);
	
	game_state(GAME_STATE_INPUT_NAME);
	
	while(1) {
		serverLoop(darnitTimeLastFrameTook());
		
		if(gamestate==GAME_STATE_QUIT)
			return 0;
		if(gamestate>=GAME_STATE_LOBBY)
			client_check_incomming();
		
		darnitRenderBegin();
		if(gamestate>=GAME_STATE_GAME)
			game_view_draw();
			
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
