#include <string.h>

#include "muon.h"
#include "view.h"
#include "client.h"
#include "chat.h"
#include "game.h"

int chat_open;

void game_state(GAME_STATE state) {
	//Game state destructors
	if(gamestate==GAME_STATE_GAME) {
		ui_event_global_remove(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_DOWN);
		ui_event_global_remove(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
		ui_event_global_remove(game_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
		ui_event_global_add(view_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
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
			ui_event_global_remove(view_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_DOWN);
			ui_event_global_add(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
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

void input_name_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	UI_PROPERTY_VALUE v;
	v=input_name_entry->get_prop(input_name_entry, UI_ENTRY_PROP_TEXT);
	memset(player_name, 0, 32);
	strncpy(player_name, v.p, 31);
	player_name[31]=0;
	printf("Player name: %s\n", player_name);
	game_state(GAME_STATE_CONNECT_SERVER);
}

void connect_server_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	UI_PROPERTY_VALUE v;
	v=connect_server_entry_host->get_prop(connect_server_entry_host, UI_ENTRY_PROP_TEXT);
	char *host=v.p;
	v=connect_server_entry_port->get_prop(connect_server_entry_port, UI_ENTRY_PROP_TEXT);
	int port=atoi(v.p);
	printf("Server: %s:%i\n", host, port);
	if(client_init(host, port)==0)
		game_state(GAME_STATE_CONNECTING);
}

void connecting_button_cancel_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	client_disconnect();
}

void ready_checkbox_toggle(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	v=widget->get_prop(widget, UI_CHECKBOX_PROP_ACTIVATED);
	client_message_send(player_id, MSG_SEND_READY, v.i, 100, NULL);
}

void game_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;

	if(widget==game_menu_button[0]) {
		game_state(GAME_STATE_QUIT);
	} else if(widget==game_menu_button[1]) {
		game_state(GAME_STATE_GAME);
	}
}

int main() {
	platform_init();

	chat_open=0;
	home_x=home_y=0;
	player_id=0;
	serverInit();
	serverStart("map.ldi", 2, 1337, 3);
	
	ui_init();
	view_init();
	chat_init();
	
	ui_event_global_add(view_draw_mouse, UI_EVENT_TYPE_MOUSE_ENTER);
	
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
		ui_events(gamestate_pane[gamestate], 1);
		darnitRenderTint(1, 1, 1, 1);
		
		darnitRenderEnd();
		
		darnitLoop();
	}
	
	return 0;
}
