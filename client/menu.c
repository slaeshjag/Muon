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
#include "client.h"
#include "chat.h"
#include "game.h"
#include "menu.h"

char *menu_sidebar_button_text_main[8]={
	"Singleplayer",
	"Multiplayer",
	"Settings",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

void menu_init() {
	int i;
	UI_PROPERTY_VALUE v={.p=NULL};
	
	//Main menu
	panelist_menu_sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, platform.screen_h, NULL);
	panelist_menu_sidebar.next=NULL;
	ui_pane_set_root_widget(panelist_menu_sidebar.pane, ui_widget_create_vbox());
	for(i=0; i<8; i++) {
		if(!menu_sidebar_button_text_main[i])
			continue;
		menu_sidebar_button[i]=ui_widget_create_button(ui_widget_create_label(font_std, menu_sidebar_button_text_main[i]));
		menu_sidebar_button[i]->event_handler->add(menu_sidebar_button[i], menu_sidebar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, menu_sidebar_button[i], 0);
	}
	ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, ui_widget_create_spacer(), 1);
	menu_sidebar_button_quit=ui_widget_create_button(ui_widget_create_label(font_std, "Quit game"));
	menu_sidebar_button_quit->event_handler->add(menu_sidebar_button_quit, menu_sidebar_button_quit_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, menu_sidebar_button_quit, 0);
	
	//Input player name
	panelist_input_name.pane=ui_pane_create(16, 32+256, 256, 96, NULL);
	ui_pane_set_root_widget(panelist_input_name.pane, ui_widget_create_vbox());
	panelist_input_name.next=NULL;
	ui_vbox_add_child(panelist_input_name.pane->root_widget, ui_widget_create_label(font_std, "Enter your player name:"), 1);
	input_name_entry=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_input_name.pane->root_widget, input_name_entry, 0);
	input_name_entry->event_handler->add(input_name_entry, input_name_button_click, UI_EVENT_TYPE_KEYBOARD);
	v.p=config.player_name;
	input_name_entry->set_prop(input_name_entry, UI_ENTRY_PROP_TEXT, v);
	input_name_button=ui_widget_create_button(ui_widget_create_label(font_std, "OK"));
	ui_vbox_add_child(panelist_input_name.pane->root_widget, input_name_button, 0);
	input_name_button->event_handler->add(input_name_button, input_name_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	
	//Monitor settings
	panelist_settings_monitor.pane=ui_pane_create(16, 16, 256, 256, NULL);
	panelist_settings_monitor.next=&panelist_input_name;
	ui_pane_set_root_widget(panelist_settings_monitor.pane, ui_widget_create_hbox());
	settings_monitor_listbox_modes=ui_widget_create_listbox(font_std);
	settings_monitor_listbox_modes->event_handler->add(settings_monitor_listbox_modes, settings_monitor_listbox_modes_changed, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_hbox_add_child(panelist_settings_monitor.pane->root_widget, settings_monitor_listbox_modes, 1);
	settings_monitor_vbox=ui_widget_create_vbox();
	ui_hbox_add_child(panelist_settings_monitor.pane->root_widget, settings_monitor_vbox, 1);
	settings_monitor_entry_w=ui_widget_create_entry(font_std);
	settings_monitor_entry_h=ui_widget_create_entry(font_std);
	ui_vbox_add_child(settings_monitor_vbox, ui_widget_create_label(font_std, "Screen width"), 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_entry_w, 0);
	ui_vbox_add_child(settings_monitor_vbox, ui_widget_create_label(font_std, "Screen height"), 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_entry_h, 0);
	settings_monitor_hbox_checkbox=ui_widget_create_hbox();
	settings_monitor_checkbox_fullscreen=ui_widget_create_checkbox();
	ui_hbox_add_child(settings_monitor_hbox_checkbox, settings_monitor_checkbox_fullscreen, 0);
	ui_hbox_add_child(settings_monitor_hbox_checkbox, ui_widget_create_label(font_std, "Fullscreen"), 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_hbox_checkbox, 0);
	ui_vbox_add_child(settings_monitor_vbox, ui_widget_create_spacer(), 1);
	settings_monitor_button_ok=ui_widget_create_button(ui_widget_create_label(font_std, "OK"));
	settings_monitor_button_ok->event_handler->add(settings_monitor_button_ok, settings_monitor_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	settings_monitor_button_cancel=ui_widget_create_button(ui_widget_create_label(font_std, "Cancel"));
	settings_monitor_button_cancel->event_handler->add(settings_monitor_button_cancel, settings_monitor_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_button_ok, 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_button_cancel, 0);
	char buf[32];
	v.p=buf;
	sprintf(buf, "%i", config.screen_w);
	settings_monitor_entry_w->set_prop(settings_monitor_entry_w, UI_ENTRY_PROP_TEXT, v);
	sprintf(buf, "%i", config.screen_h);
	settings_monitor_entry_h->set_prop(settings_monitor_entry_h, UI_ENTRY_PROP_TEXT, v);
	v.i=config.fullscreen;
	settings_monitor_checkbox_fullscreen->set_prop(settings_monitor_checkbox_fullscreen, UI_CHECKBOX_PROP_ACTIVATED, v);
	DARNIT_VIDEOMODE **mode;
	for(mode=videomodes; mode&&*mode; mode+=sizeof(void *)) {
		sprintf(buf, "%ix%i", (*mode)->w, (*mode)->h);
		ui_listbox_add(settings_monitor_listbox_modes, buf);
	}
		
	//Connect to server
	v.p=NULL;
	panelist_connect_server.pane=ui_pane_create(16, 16, 256, 128, NULL);
	ui_pane_set_root_widget(panelist_connect_server.pane, ui_widget_create_vbox());
	panelist_connect_server.next=NULL;
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, ui_widget_create_label(font_std, "Connect to a server"), 1);
	connect_server_entry_host=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_entry_host, 0);
	connect_server_entry_port=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_entry_port, 0);
	connect_server_entry_host->event_handler->add(connect_server_entry_host, connect_server_button_click, UI_EVENT_TYPE_KEYBOARD);
	connect_server_button=ui_widget_create_button(ui_widget_create_label(font_std, "Connect"));
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_button, 0);
	connect_server_button->event_handler->add(connect_server_button, connect_server_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
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
	
	//In-game menu
	panelist_game_menu.pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-128, 256, 256, NULL);
	ui_pane_set_root_widget(panelist_game_menu.pane, ui_widget_create_vbox());
	panelist_game_menu.next=&panelist_chat;
	ui_vbox_add_child(panelist_game_menu.pane->root_widget, ui_widget_create_label(font_std, "Muon\n===="), 0);
	game_menu_button[0]=ui_widget_create_button(ui_widget_create_label(font_std, "Disconnect"));
	game_menu_button[1]=ui_widget_create_button(ui_widget_create_label(font_std, "Quit game"));
	game_menu_button[2]=ui_widget_create_button(ui_widget_create_label(font_std, "Return to game"));
	for(i=0; i<2; i++) {
		ui_vbox_add_child(panelist_game_menu.pane->root_widget, game_menu_button[i], 0);
		game_menu_button[i]->event_handler->add(game_menu_button[i], game_menu_button_click, UI_EVENT_TYPE_UI);
	}
	ui_vbox_add_child(panelist_game_menu.pane->root_widget, ui_widget_create_spacer(), 1);
	ui_vbox_add_child(panelist_game_menu.pane->root_widget, game_menu_button[2], 0);
	game_menu_button[2]->event_handler->add(game_menu_button[2], game_menu_button_click, UI_EVENT_TYPE_UI);
}

//Main menu
void menu_sidebar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	if(widget==menu_sidebar_button[1]) {
		panelist_menu_sidebar.next=&panelist_connect_server;
		ui_selected_widget=connect_server_entry_host;
	} else if(widget==menu_sidebar_button[2]) {
		//panelist_menu_sidebar.next=&panelist_input_name;
		panelist_menu_sidebar.next=&panelist_settings_monitor;
		ui_selected_widget=input_name_entry;
	} else {
		panelist_menu_sidebar.next=NULL;
		ui_selected_widget=NULL;
	}
}

void menu_sidebar_button_quit_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	game_state(GAME_STATE_QUIT);
}

//Settings
void settings_monitor_listbox_modes_changed(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	char buf[32];
	UI_PROPERTY_VALUE v;
	v=widget->get_prop(widget, UI_LISTBOX_PROP_SELECTED);
	int i=v.i;
	v.p=buf;
	sprintf(buf, "%i", videomodes[i]->w);
	settings_monitor_entry_w->set_prop(settings_monitor_entry_w, UI_ENTRY_PROP_TEXT, v);
	sprintf(buf, "%i", videomodes[i]->h);
	settings_monitor_entry_h->set_prop(settings_monitor_entry_h, UI_ENTRY_PROP_TEXT, v);
}

void settings_monitor_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget==settings_monitor_button_ok) {
		UI_PROPERTY_VALUE v;
		v=settings_monitor_entry_w->get_prop(settings_monitor_entry_w, UI_ENTRY_PROP_TEXT);
		config.screen_w=atoi(v.p);
		v=settings_monitor_entry_h->get_prop(settings_monitor_entry_h, UI_ENTRY_PROP_TEXT);
		config.screen_h=atoi(v.p);
		v=settings_monitor_checkbox_fullscreen->get_prop(settings_monitor_checkbox_fullscreen, UI_CHECKBOX_PROP_ACTIVATED);
		config.fullscreen=v.i;
		platform_config_write();
	}
	panelist_menu_sidebar.next=NULL;
}

void input_name_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(!(type==UI_EVENT_TYPE_UI_WIDGET_ACTIVATE||(type==UI_EVENT_TYPE_KEYBOARD_PRESS&&e->keyboard->keysym==KEY(RETURN))))
		return;
	UI_PROPERTY_VALUE v;
	v=input_name_entry->get_prop(input_name_entry, UI_ENTRY_PROP_TEXT);
	if(!strlen(v.p))
		return;
	memset(config.player_name, 0, 32);
	strncpy(config.player_name, v.p, 31);
	config.player_name[31]=0;
	platform_config_write();
	//printf("Player name: %s\n", player_name);
	panelist_menu_sidebar.next=NULL;
}

void connect_server_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(!(type==UI_EVENT_TYPE_UI_WIDGET_ACTIVATE||(type==UI_EVENT_TYPE_KEYBOARD_PRESS&&e->keyboard->keysym==KEY(RETURN))))
		return;
	UI_PROPERTY_VALUE v;
	v=connect_server_entry_host->get_prop(connect_server_entry_host, UI_ENTRY_PROP_TEXT);
	char *host=v.p;
	v=connect_server_entry_port->get_prop(connect_server_entry_port, UI_ENTRY_PROP_TEXT);
	int port=atoi(v.p);
	//printf("Server: %s:%i\n", host, port);
	if(client_init(host, port)==0)
		game_state(GAME_STATE_CONNECTING);
}

void connecting_button_cancel_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	client_disconnect();
}

void menu_render() {
	
}

//In-game menu
void game_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	if(widget==game_menu_button[0]) {
		client_disconnect();
		game_state(GAME_STATE_MENU);
		panelist_menu_sidebar.next=NULL;
	} else if(widget==game_menu_button[1]) {
		game_state(GAME_STATE_QUIT);
	} else if(widget==game_menu_button[2]) {
		game_state(GAME_STATE_GAME);
	}
}
