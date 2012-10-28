#include <string.h>

#include "muon.h"
#include "view.h"
#include "client.h"

UI_WIDGET *chatbuf;
UI_WIDGET *textentry;

char textbuf[256]={};
char *textp=textbuf;

void input_name_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;	
	UI_PROPERTY_VALUE v;
	v=input_name_entry->get_prop(input_name_entry, UI_ENTRY_PROP_TEXT);
	memset(player_name, 0, 32);
	strncpy(player_name, v.p, 31);
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
	state=GAME_STATE_GAME;
}

int main() {
	if(darnitInit("Muon", "muon")==NULL) {
		fprintf(stderr, "lol\n");
		return 1;
	}
	platform=darnitPlatformGet();
	DARNIT_MOUSE mouse;
	
	serverInit("map.ldi", 1, 1337);
	state=GAME_STATE_INPUT_NAME;
	
	ui_init();
	view_init();
	
	/*UI_PANE *chatpane=ui_pane_create(10, 10, 256, 256, NULL);
	struct UI_PANE_LIST panelist={chatpane, NULL};
	chatbuf=ui_widget_create_label(font_std, "");
	UI_WIDGET *vbox=ui_widget_create_vbox();
	UI_WIDGET *sendbutton=ui_widget_create_button_text("Send");
	textentry=ui_widget_create_entry(font_std);
	ui_pane_set_root_widget(chatpane, vbox);
	ui_vbox_add_child(vbox, ui_widget_create_label(font_std, "Muon Chat"), 0);
	ui_vbox_add_child(vbox, chatbuf, 1);
	ui_vbox_add_child(vbox, textentry, 0);
	ui_vbox_add_child(vbox, sendbutton, 0);
	sendbutton->event_handler->add(sendbutton, sendbutton_click , UI_EVENT_TYPE_UI);*/
	
	while(1) {
		serverLoop(darnitTimeLastFrameTook());
		switch(state) {
			case GAME_STATE_INPUT_NAME:
				darnitRenderBegin();
				darnitRenderTint(1, 0, 0, 1);
				ui_events(&panelist_input_name, 1);
				darnitRenderTint(1, 1, 1, 1);
				darnitRenderEnd();
				break;
			case GAME_STATE_CONNECT_SERVER:
				darnitRenderBegin();
				darnitRenderTint(1, 0, 0, 1);
				ui_events(&panelist_connect_server, 1);
				darnitRenderTint(1, 1, 1, 1);
				darnitRenderEnd();
				break;
			case GAME_STATE_GAME:
				client_check_incomming();
				mouse=darnitMouseGet();
				view_scroll(mouse);
				darnitRenderBegin();
				view_draw();
				darnitRenderEnd();
				break;
		}
		
		darnitLoop();
		//printf("%i           \r", darnitFPSGet());
	}
	
	return 0;
}
