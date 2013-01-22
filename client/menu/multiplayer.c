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

#include "../muon.h"
#include "../client.h"
#include "multiplayer.h"

void menu_multiplayer_init() {
	UI_PROPERTY_VALUE v={.p=NULL};
	
	//Host server
	panelist_multiplayer_host.pane=ui_pane_create(16, 16, 480, 256, NULL);
	panelist_multiplayer_host.next=NULL;
	ui_pane_set_root_widget(panelist_multiplayer_host.pane, ui_widget_create_hbox());
	multiplayer_host_listbox_maps=ui_widget_create_listbox(font_std);
	ui_hbox_add_child(panelist_multiplayer_host.pane->root_widget, multiplayer_host_listbox_maps, 1);
	multiplayer_host_vbox=ui_widget_create_vbox();
	ui_hbox_add_child(panelist_multiplayer_host.pane->root_widget, multiplayer_host_vbox, 1);
	ui_vbox_add_child(multiplayer_host_vbox, ui_widget_create_label(font_std, T("Port")), 0);
	multiplayer_host_entry_port=ui_widget_create_entry(font_std);
	ui_vbox_add_child(multiplayer_host_vbox, multiplayer_host_entry_port, 0);
	v.p=CLIENT_DEFAULT_PORT;
	multiplayer_host_entry_port->set_prop(multiplayer_host_entry_port, UI_ENTRY_PROP_TEXT, v);
	ui_vbox_add_child(multiplayer_host_vbox, ui_widget_create_label(font_std, T("Number of players")), 0);
	multiplayer_host_slider_players=ui_widget_create_slider(4);
	v.i=config.players-1;
	multiplayer_host_slider_players->set_prop(multiplayer_host_slider_players, UI_SLIDER_PROP_VALUE, v);
	ui_vbox_add_child(multiplayer_host_vbox, multiplayer_host_slider_players, 0);
	ui_vbox_add_child(multiplayer_host_vbox, ui_widget_create_label(font_std, T("Game speed")), 0);
	multiplayer_host_slider_gamespeed=ui_widget_create_slider(10);
	v.i=config.gamespeed-1;
	multiplayer_host_slider_gamespeed->set_prop(multiplayer_host_slider_gamespeed, UI_SLIDER_PROP_VALUE, v);
	ui_vbox_add_child(multiplayer_host_vbox, multiplayer_host_slider_gamespeed, 0);
	ui_vbox_add_child(multiplayer_host_vbox, ui_widget_create_spacer(), 1);
	multiplayer_host_button_host=ui_widget_create_button_text(font_std, T("Host"));
	ui_vbox_add_child(multiplayer_host_vbox, multiplayer_host_button_host, 0);
	multiplayer_host_button_host->event_handler->add(multiplayer_host_button_host, multiplayer_host_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	menu_multiplayer_host_maps_reload();
	
	//Join server
	v.p=NULL;
	panelist_multiplayer_join.pane=ui_pane_create(16, 16, 256, 128, NULL);
	ui_pane_set_root_widget(panelist_multiplayer_join.pane, ui_widget_create_vbox());
	panelist_multiplayer_join.next=NULL;
	ui_vbox_add_child(panelist_multiplayer_join.pane->root_widget, ui_widget_create_label(font_std, T("Join a server")), 1);
	multiplayer_join_entry_host=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_multiplayer_join.pane->root_widget, multiplayer_join_entry_host, 0);
	multiplayer_join_entry_port=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_multiplayer_join.pane->root_widget, multiplayer_join_entry_port, 0);
	multiplayer_join_entry_host->event_handler->add(multiplayer_join_entry_host, multiplayer_join_button_click, UI_EVENT_TYPE_KEYBOARD_PRESS);
	multiplayer_join_button=ui_widget_create_button_text(font_std, T("Join"));
	ui_vbox_add_child(panelist_multiplayer_join.pane->root_widget, multiplayer_join_button, 0);
	multiplayer_join_button->event_handler->add(multiplayer_join_button, multiplayer_join_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	v.p="localhost";
	multiplayer_join_entry_host->set_prop(multiplayer_join_entry_host, UI_ENTRY_PROP_TEXT, v);
	v.p=CLIENT_DEFAULT_PORT;
	multiplayer_join_entry_port->set_prop(multiplayer_join_entry_port, UI_ENTRY_PROP_TEXT, v);
	
	//Connecting
	panelist_multiplayer_connecting.pane=ui_pane_create(platform.screen_w/2-90, platform.screen_h/2-32, 180, 64, NULL);
	panelist_multiplayer_connecting.next=NULL;
	ui_pane_set_root_widget(panelist_multiplayer_connecting.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_multiplayer_connecting.pane->root_widget, ui_widget_create_label(font_std, T("Connecting to server")), 1);
	multiplayer_connecting_button_cancel=ui_widget_create_button_text(font_std, T("Cancel"));
	ui_vbox_add_child(panelist_multiplayer_connecting.pane->root_widget, multiplayer_connecting_button_cancel, 0);
	multiplayer_connecting_button_cancel->event_handler->add(multiplayer_connecting_button_cancel, multiplayer_connecting_button_cancel_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
}

void menu_multiplayer_host_maps_reload() {
	DARNIT_FILE *f;
	DARNIT_DIR_LIST *dir, *d;
	char buf[256];
	int dirs;
	ui_listbox_clear(multiplayer_host_listbox_maps);
	dir=d_file_list(mapdir, DARNIT_FILESYSTEM_TYPE_READ|DARNIT_FILESYSTEM_TYPE_WRITE, &dirs);
	for(d=dir; d; d=d->next) {
		if(d->file&&ui_listbox_index_of(multiplayer_host_listbox_maps, d->fname)==-1) {
			sprintf(buf, "%s/%s", mapdir, d->fname);
			f=d_file_open(buf, "rb");
			d_file_read(buf, 8, f);
			d_file_close(f);
			if(memcmp(buf, ldimagic, 8))
				continue;
			ui_listbox_add(multiplayer_host_listbox_maps, d->fname);
		}
	}
	d_file_list_free(dir);
}

void multiplayer_host_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	int port, players, speed;
	char buf[256];
	v=multiplayer_host_entry_port->get_prop(multiplayer_host_entry_port, UI_ENTRY_PROP_TEXT);
	port=atoi(v.p);
	v=multiplayer_host_slider_players->get_prop(multiplayer_host_slider_players, UI_SLIDER_PROP_VALUE);
	players=v.i+1;
	v=multiplayer_host_slider_gamespeed->get_prop(multiplayer_host_slider_gamespeed, UI_SLIDER_PROP_VALUE);
	speed=v.i+1;
	
	config.gamespeed=speed;
	config.players=players;
	
	v=multiplayer_host_listbox_maps->get_prop(multiplayer_host_listbox_maps, UI_LISTBOX_PROP_SELECTED);
	if(v.i==-1)
		return;
	sprintf(buf, "%s/%s", mapdir, ui_listbox_get(multiplayer_host_listbox_maps, v.i));
	server_stop();
	if(!server_start(buf, players, port, speed)) {
		const char *error;
		if((error=server_error_pop()))
			ui_messagebox(font_std, T(error));
		return;
	}
	if(client_init("localhost", port)==0)
		game_state(GAME_STATE_CONNECTING);
	else {
		ui_messagebox(font_std, T("Unable to connect to server."));
		server_stop();
	}
}

void multiplayer_join_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if((type==UI_EVENT_TYPE_KEYBOARD_PRESS&&e->keyboard->keysym!=KEY(RETURN)))
		return;
	UI_PROPERTY_VALUE v;
	v=multiplayer_join_entry_host->get_prop(multiplayer_join_entry_host, UI_ENTRY_PROP_TEXT);
	char *host=v.p;
	v=multiplayer_join_entry_port->get_prop(multiplayer_join_entry_port, UI_ENTRY_PROP_TEXT);
	int port=atoi(v.p);
	if(client_init(host, port)==0)
		game_state(GAME_STATE_CONNECTING);
}

void multiplayer_connecting_button_cancel_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	client_disconnect(-1);
}
