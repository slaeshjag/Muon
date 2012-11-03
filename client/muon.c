#include <string.h>

#include "muon.h"
#include "view.h"
#include "client.h"
#include "chat.h"

#define RENDER_MOUSE darnitRenderBlendingEnable(); darnitRenderTileBlit(mouse_tilesheet, 0, mouse.x, mouse.y); darnitRenderBlendingDisable()

int chat_open;

void game_state(GAME_STATE state) {
	switch(state) {
		case GAME_STATE_INPUT_NAME:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			ui_selected_widget=input_name_entry;
			break;
		case GAME_STATE_CONNECT_SERVER:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			ui_selected_widget=connect_server_entry_host;
			break;
		case GAME_STATE_CONNECTING:
			ui_selected_widget=NULL;
		case GAME_STATE_LOBBY:
			darnitRenderClearColorSet(0x0, 0x0, 0x0);
			//chat_show(gamestate_pane[GAME_STATE_LOBBY]);
			//ui_selected_widget=chat_entry;
			break;
		case GAME_STATE_GAME:
			darnitRenderClearColorSet(0x7f, 0x7f, 0x7f);
			ui_selected_widget=NULL;
			//darnitInputGrab();
			break;
		case GAME_STATE_GAME_MENU:
			darnitInputUngrab();
			break;
		case GAME_STATE_QUIT:
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

void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	int i, do_build=-1;
	UI_PROPERTY_VALUE wv=widget->get_prop(widget, UI_BUTTON_PROP_CHILD);
	UI_PROPERTY_VALUE pv=game_sidebar_progress_build->get_prop(game_sidebar_progress_build, UI_PROGRESSBAR_PROP_PROGRESS);
	for(i=0; i<4; i++) {
		if(widget==game_sidebar_button_build[i]&&wv.p!=game_sidebar_progress_build) {
			do_build=i; //just make sure to cancel all other buildings first
		} else if(widget==game_sidebar_button_build[i]&&pv.i==100) {
			printf("Place building\n");
			building_place=i;
		} else {
			UI_PROPERTY_VALUE v={.p=game_sidebar_label_build[i]};
			game_sidebar_button_build[i]->set_prop(game_sidebar_button_build[i], UI_BUTTON_PROP_CHILD, v);
			client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+i, MSG_BUILDING_STOP, NULL);
		}
	}
	if(do_build!=-1) {
		client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+do_build, MSG_BUILDING_START, NULL);
		UI_PROPERTY_VALUE v={.p=game_sidebar_progress_build};
		widget->set_prop(widget, UI_BUTTON_PROP_CHILD, v);
		v.i=0;
		game_sidebar_progress_build->set_prop(game_sidebar_progress_build, UI_PROGRESSBAR_PROP_PROGRESS, v);
	}
	//client_message_send(player_id, MSG_SEND_CHAT, 0, 8, "ostkaka!");
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
	
	DARNIT_MOUSE mouse;
	DARNIT_KEYS buttons;
	chat_open=0;
	home_x=home_y=0;
	player_id=0;
	serverInit();
	serverStart("map.ldi", 2, 1337, 3);
	game_state(GAME_STATE_INPUT_NAME);
	
	ui_init();
	view_init();
	chat_init();
	
	while(1) {
		serverLoop(darnitTimeLastFrameTook());
		buttons=darnitButtonGet();
		mouse=darnitMouseGet();
		
		if(gamestate==GAME_STATE_QUIT)
			return 0;
		if(gamestate>=GAME_STATE_LOBBY) {
			if(client_check_incomming()==-1) {
				fprintf(stderr, "Server disconnected!\n");
				game_state(GAME_STATE_CONNECT_SERVER);
			}
		}
		
		darnitRenderBegin();
		darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
		
		ui_events(gamestate_pane[gamestate], 1);
		
		darnitRenderTint(1, 1, 1, 1);
		//render mouse
		darnitRenderBlendingEnable();
		darnitRenderTileBlit(mouse_tilesheet, 0, mouse.x, mouse.y);
		darnitRenderBlendingDisable();
		
		darnitRenderEnd();
		
		/*switch(state) {
			case GAME_STATE_INPUT_NAME:
				darnitRenderBegin();
				darnitRenderTint(1, 0, 0, 1);
				ui_events(&panelist_input_name, 1);
				darnitRenderTint(1, 1, 1, 1);
				RENDER_MOUSE;
				darnitRenderEnd();
				if(buttons.start)
					state=GAME_STATE_QUIT;
				break;
			case GAME_STATE_CONNECT_SERVER:
				darnitRenderBegin();
				darnitRenderTint(1, 0, 0, 1);
				ui_events(&panelist_connect_server, 1);
				darnitRenderTint(1, 1, 1, 1);
				RENDER_MOUSE;
				darnitRenderEnd();
				if(buttons.start)
					state=GAME_STATE_QUIT;
				break;
			case GAME_STATE_CONNECTING:
				darnitRenderBegin();
				darnitRenderTint(1, 0, 0, 1);
				ui_pane_render(panelist_connect_server.pane);
				ui_events(&panelist_connecting, 1);
				darnitRenderTint(1, 1, 1, 1);
				RENDER_MOUSE;
				darnitRenderEnd();
				break;
			case GAME_STATE_LOBBY:
				if(client_check_incomming()==-1) {
					fprintf(stderr, "Server disconnected!\n");
					state=GAME_STATE_CONNECT_SERVER;
					break;
				}
				darnitRenderBegin();
				//darnitRenderTint(1, 0, 0, 1);
				darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
				ui_events(&panelist_countdown, 1);
				darnitRenderTint(1, 1, 1, 1);
				RENDER_MOUSE;
				darnitRenderEnd();
				break;
			case GAME_STATE_GAME:
				if(client_check_incomming()==-1) {
					fprintf(stderr, "Server disconnected!\n");
					state=GAME_STATE_CONNECT_SERVER;
					darnitInputUngrab();
					break;
				}
				view_scroll(&mouse, &buttons);
				darnitRenderBegin();
				view_draw(&mouse);
				darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
				ui_events(&panelist_game_sidebar, 1);
				darnitRenderTint(1, 1, 1, 1);
				RENDER_MOUSE;
				darnitRenderEnd();
				if(buttons.start) {
					state=GAME_STATE_GAME_MENU;
					darnitInputUngrab();
				}
				if(buttons.x) {
					if(!chat_open)
						chat_toggle(&panelist_game_sidebar);
					chat_open=1;
				} else {
					chat_open=0;
				}
				break;
			case GAME_STATE_GAME_MENU:
				if(client_check_incomming()==-1) {
					fprintf(stderr, "Server disconnected!\n");
					state=GAME_STATE_CONNECT_SERVER;
					break;
				}
				darnitRenderBegin();
				view_draw(&mouse);
				darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
				ui_pane_render(panelist_game_sidebar.pane);
				ui_events(&panelist_game_menu, 1);
				darnitRenderTint(1, 1, 1, 1);
				RENDER_MOUSE;
				darnitRenderEnd();
				break;
			case GAME_STATE_QUIT:
				darnitInputUngrab();
				return 0;
		}*/
		
		darnitLoop();
		//printf("%i           \r", darnitFPSGet());
	}
	
	return 0;
}
