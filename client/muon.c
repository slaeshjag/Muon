#include <string.h>

#include "muon.h"
#include "view.h"
#include "client.h"

void input_name_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;	
	UI_PROPERTY_VALUE v;
	v=input_name_entry->get_prop(input_name_entry, UI_ENTRY_PROP_TEXT);
	memset(player_name, 0, 32);
	strncpy(player_name, v.p, 31);
	if(!strnlen(player_name, 32))
		return;
	printf("Player name: %s\n", player_name);
	state=GAME_STATE_CONNECT_SERVER;
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
	client_init(host, port);
	state=GAME_STATE_COUNTDOWN;
}

void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	int i;
	for(i=0; i<4; i++) {
		if(widget==game_sidebar_button_build[i]) {
			client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+i, MSG_BUILDING_START, NULL);
		}
	}
	//client_message_send(player_id, MSG_SEND_CHAT, 0, 8, "ostkaka!");
}

void game_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;

	if(widget==game_menu_button[0]) {
		state=GAME_STATE_QUIT;
	} else if(widget==game_menu_button[1]) {
		state=GAME_STATE_GAME;
		darnitInputGrab();
	}
}

int main() {
	if(darnitInit("Muon", "muon")==NULL) {
		fprintf(stderr, "Failed to initialize libdarnit!\n");
		return 1;
	}
	platform=darnitPlatformGet();
	DARNIT_MOUSE mouse;
	
	serverInit();
	serverStart("map.ldi", 1, 1337);
	state=GAME_STATE_INPUT_NAME;
	
	ui_init();
	view_init();
	
	while(1) {
		serverLoop(darnitTimeLastFrameTook());
		DARNIT_KEYS buttons=darnitButtonGet();
		switch(state) {
			case GAME_STATE_INPUT_NAME:
				darnitRenderBegin();
				darnitRenderTint(1, 0, 0, 1);
				ui_events(&panelist_input_name, 1);
				darnitRenderTint(1, 1, 1, 1);
				darnitRenderEnd();
				if(buttons.select)
					state=GAME_STATE_QUIT;
				break;
			case GAME_STATE_CONNECT_SERVER:
				darnitRenderBegin();
				darnitRenderTint(1, 0, 0, 1);
				ui_events(&panelist_connect_server, 1);
				darnitRenderTint(1, 1, 1, 1);
				darnitRenderEnd();
				if(buttons.select)
					state=GAME_STATE_QUIT;
				break;
			case GAME_STATE_COUNTDOWN:
				if(client_check_incomming()==-1) {
					fprintf(stderr, "Server disconnected!\n");
					state=GAME_STATE_CONNECT_SERVER;
					break;
				}
				darnitRenderBegin();
				darnitRenderTint(1, 0, 0, 1);
				ui_events(&panelist_countdown, 1);
				darnitRenderTint(1, 1, 1, 1);
				darnitRenderEnd();
				break;
			case GAME_STATE_GAME:
				if(client_check_incomming()==-1) {
					fprintf(stderr, "Server disconnected!\n");
					state=GAME_STATE_CONNECT_SERVER;
					darnitInputUngrab();
					break;
				}
				mouse=darnitMouseGet();
				view_scroll(mouse);
				darnitRenderBegin();
				view_draw();
				darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
				ui_events(&panelist_game_sidebar, 1);
				darnitRenderTint(1, 1, 1, 1);
				darnitRenderEnd();
				if(buttons.select) {
					state=GAME_STATE_GAME_MENU;
					darnitInputUngrab();
				}
				break;
			case GAME_STATE_GAME_MENU:
				if(client_check_incomming()==-1) {
					fprintf(stderr, "Server disconnected!\n");
					state=GAME_STATE_CONNECT_SERVER;
					break;
				}
				darnitRenderBegin();
				view_draw();
				darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
				ui_pane_render(panelist_game_sidebar.pane);
				ui_events(&panelist_game_menu, 1);
				darnitRenderTint(1, 1, 1, 1);
				darnitRenderEnd();
				break;
			case GAME_STATE_QUIT:
				darnitInputUngrab();
				return 0;
		}
		
		darnitLoop();
		//printf("%i           \r", darnitFPSGet());
	}
	
	return 0;
}
