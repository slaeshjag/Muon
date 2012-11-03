#include <string.h>

#include "muon.h"
#include "view.h"
#include "game.h"
//#include "client.h"
//#include "engine.h"
#include "chat.h"

void view_init() {
	//TODO: lots of breaking out to separate functions, game menu and lobby for example
	font_std=darnitFontLoad("../res/FreeMonoBold.ttf", 12, 512, 512);
	mouse_tilesheet=darnitRenderTilesheetLoad("../res/mouse.png", 16, 16, DARNIT_PFORMAT_RGB5A1);
	int i;
	
	//Input player name
	panelist_input_name.pane=ui_pane_create(16, 16, 256, 96, NULL);
	ui_pane_set_root_widget(panelist_input_name.pane, ui_widget_create_vbox());
	panelist_input_name.next=NULL;
	ui_vbox_add_child(panelist_input_name.pane->root_widget, ui_widget_create_label(font_std, "Enter your player name:"), 1);
	input_name_entry=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_input_name.pane->root_widget, input_name_entry, 0);
	input_name_button=ui_widget_create_button(ui_widget_create_label(font_std, "OK"));
	ui_vbox_add_child(panelist_input_name.pane->root_widget, input_name_button, 0);
	input_name_button->event_handler->add(input_name_button, input_name_button_click, UI_EVENT_TYPE_UI);
	
	//Connect to server
	UI_PROPERTY_VALUE v={.p=NULL};
	panelist_connect_server.pane=ui_pane_create(16, 16, 256, 128, NULL);
	ui_pane_set_root_widget(panelist_connect_server.pane, ui_widget_create_vbox());
	panelist_connect_server.next=NULL;
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, ui_widget_create_label(font_std, "Connect to a server"), 1);
	connect_server_entry_host=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_entry_host, 0);
	connect_server_entry_port=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_entry_port, 0);
	connect_server_button=ui_widget_create_button(ui_widget_create_label(font_std, "Connect"));
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_button, 0);
	connect_server_button->event_handler->add(connect_server_button, connect_server_button_click, UI_EVENT_TYPE_UI);
	v.p="localhost";
	connect_server_entry_host->set_prop(connect_server_entry_host, UI_ENTRY_PROP_TEXT, v);
	v.p="1337";
	connect_server_entry_port->set_prop(connect_server_entry_port, UI_ENTRY_PROP_TEXT, v);
	
	//Connecting
	panelist_connecting.pane=ui_pane_create(platform.screen_w/2-90, platform.screen_h/2-32, 180, 64, NULL);
	panelist_connecting.next=NULL;
	ui_pane_set_root_widget(panelist_connecting.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_connecting.pane->root_widget, ui_widget_create_label(font_std, "Connecting to server"), 1);
	connecting_button_cancel=ui_widget_create_button_text("Cancel");
	ui_vbox_add_child(panelist_connecting.pane->root_widget, connecting_button_cancel, 0);
	connecting_button_cancel->event_handler->add(connecting_button_cancel, connecting_button_cancel_click, UI_EVENT_TYPE_UI);
	
	//Lobby
	panelist_lobby.pane=ui_pane_create(platform.screen_w/2-64, platform.screen_h/2-32, 128, 64, NULL);
	strcpy(lobby_countdown_text, "Downloading map");
	ui_pane_set_root_widget(panelist_lobby.pane, ui_widget_create_vbox());
	lobby_countdown_label=ui_widget_create_label(font_std, lobby_countdown_text);
	lobby_progress_map=ui_widget_create_progressbar(font_std);
	ui_vbox_add_child(panelist_lobby.pane->root_widget, lobby_countdown_label, 1);
	ui_vbox_add_child(panelist_lobby.pane->root_widget, lobby_progress_map, 0);
	v.i=0;
	lobby_progress_map->set_prop(lobby_progress_map, UI_PROGRESSBAR_PROP_PROGRESS, v);	
	panelist_lobby.next=&panelist_lobby_ready;
	panelist_lobby_ready.pane=ui_pane_create(16, 16, 64, 32, NULL);
	ui_pane_set_root_widget(panelist_lobby_ready.pane, ui_widget_create_hbox());
	lobby_checkbox_ready=ui_widget_create_checkbox();
	ui_hbox_add_child(panelist_lobby_ready.pane->root_widget, lobby_checkbox_ready, 0);
	ui_hbox_add_child(panelist_lobby_ready.pane->root_widget, ui_widget_create_label(font_std, "Ready"), 1);
	
	//Game
	game_view_init();
	
	//Game menu
	panelist_game_menu.pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-128, 256, 256, NULL);
	ui_pane_set_root_widget(panelist_game_menu.pane, ui_widget_create_vbox());
	panelist_game_menu.next=&panelist_chat;
	ui_vbox_add_child(panelist_game_menu.pane->root_widget, ui_widget_create_label(font_std, "Muon\n===="), 0);
	game_menu_button[0]=ui_widget_create_button(ui_widget_create_label(font_std, "Quit game"));
	game_menu_button[1]=ui_widget_create_button(ui_widget_create_label(font_std, "Return to game"));
	for(i=0; i<2; i++) {
		ui_vbox_add_child(panelist_game_menu.pane->root_widget, game_menu_button[i], 0);
		game_menu_button[i]->event_handler->add(game_menu_button[i], game_menu_button_click, UI_EVENT_TYPE_UI);
	}
	
	gamestate_pane[0]=&panelist_input_name;
	gamestate_pane[1]=&panelist_connect_server;
	gamestate_pane[2]=&panelist_connecting;
	gamestate_pane[3]=&panelist_lobby;
	gamestate_pane[4]=&panelist_game_sidebar;
	gamestate_pane[5]=&panelist_game_menu;
	gamestate_pane[6]=NULL;
}

void view_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	float r, g, b, a;
	darnitRenderTintGet(&r, &g, &b, &a);
	darnitRenderTint(1, 1, 1, 1);
	darnitRenderBlendingEnable();
	darnitRenderTileBlit(mouse_tilesheet, 0, e->mouse->x, e->mouse->y);
	darnitRenderBlendingDisable();
	darnitRenderTint(r, g, b, a);
}
