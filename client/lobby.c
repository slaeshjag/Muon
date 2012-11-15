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
#include "map.h"
#include "lobby.h"

void lobby_init() {
	UI_PROPERTY_VALUE v;
	panelist_lobby_players.pane=ui_pane_create(16, 16, 256, 128, NULL);
	panelist_lobby_players.next=&panelist_lobby_download;
	ui_pane_set_root_widget(panelist_lobby_players.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, ui_widget_create_label(font_std, "Players"), 0);
	lobby_players_listbox=ui_widget_create_listbox(font_std);
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, lobby_players_listbox, 1);
	lobby_players_hbox=ui_widget_create_hbox();
	lobby_players_checkbox_ready=ui_widget_create_checkbox();
	ui_hbox_add_child(lobby_players_hbox, lobby_players_checkbox_ready, 0);
	ui_hbox_add_child(lobby_players_hbox, ui_widget_create_label(font_std, "Ready"), 0);
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, ui_widget_create_spacer_size(1, 8), 0);
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, lobby_players_hbox, 0);
	
	panelist_lobby_download.pane=ui_pane_create(platform.screen_w/2-64, platform.screen_h/2-32, 128, 64, NULL);
	panelist_lobby_download.next=&panelist_lobby_map;
	ui_pane_set_root_widget(panelist_lobby_download.pane, ui_widget_create_vbox());
	lobby_download_progress=ui_widget_create_progressbar(font_std);
	ui_vbox_add_child(panelist_lobby_download.pane->root_widget, ui_widget_create_label(font_std, "Downloading map"), 1);
	ui_vbox_add_child(panelist_lobby_download.pane->root_widget, lobby_download_progress, 0);
	v.i=0;
	lobby_download_progress->set_prop(lobby_download_progress, UI_PROGRESSBAR_PROP_PROGRESS, v);
	
	panelist_lobby_map.pane=ui_pane_create(platform.screen_w-128-16, 16, 128, 152, NULL);
	lobby_map_imageview=ui_widget_create_imageview_raw(128-2*UI_PADDING, 128-2*UI_PADDING, DARNIT_PFORMAT_RGB5A1);
	lobby_map_label=ui_widget_create_label(font_std, "Map");
	ui_pane_set_root_widget(panelist_lobby_map.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_lobby_map.pane->root_widget, lobby_map_label, 1);
	ui_vbox_add_child(panelist_lobby_map.pane->root_widget, lobby_map_imageview, 0);
	
	//panelist_lobby.next=&panelist_lobby_ready;
	/*panelist_lobby_ready.pane=ui_pane_create(16, 16, 64, 32, NULL);
	ui_pane_set_root_widget(panelist_lobby_ready.pane, ui_widget_create_hbox());
	lobby_checkbox_ready=ui_widget_create_checkbox();
	ui_hbox_add_child(panelist_lobby_ready.pane->root_widget, lobby_checkbox_ready, 0);
	ui_hbox_add_child(panelist_lobby_ready.pane->root_widget, ui_widget_create_label(font_std, "Ready"), 1);*/
}

void lobby_open() {
	int i;
	UI_PROPERTY_VALUE v={.i=0};
	lobby_players_checkbox_ready->set_prop(lobby_players_checkbox_ready, UI_CHECKBOX_PROP_ACTIVATED, v);
	lobby_players_checkbox_ready->event_handler->add(lobby_players_checkbox_ready, lobby_players_checkbox_ready_toggle, UI_EVENT_TYPE_UI);
	ui_listbox_clear(lobby_players_listbox);
	for(i=0; i<players; i++)
		ui_listbox_add(lobby_players_listbox, "");
	panelist_lobby_players.next=&panelist_lobby_download;
	chat_show(gamestate_pane[GAME_STATE_LOBBY]);
	ui_selected_widget=chat_entry;
}

void lobby_close() {
	UI_PROPERTY_VALUE v={.i=0};
	lobby_players_checkbox_ready->set_prop(lobby_players_checkbox_ready, UI_CHECKBOX_PROP_ACTIVATED, v);
	lobby_players_checkbox_ready->event_handler->remove(lobby_players_checkbox_ready, lobby_players_checkbox_ready_toggle, UI_EVENT_TYPE_UI);
}

void lobby_map_preview_generate() {
	UI_PROPERTY_VALUE v;
	v.p=(void *)darnitMapPropGet(map->prop, "name");
	if(v.p)
		lobby_map_label->set_prop(lobby_map_label, UI_LABEL_PROP_TEXT, v);
	v=lobby_map_imageview->get_prop(lobby_map_imageview, UI_IMAGEVIEW_PROP_TILESHEET);
	map_minimap_update(v.p, lobby_map_imageview->w, lobby_map_imageview->h, 0);
}

void lobby_join(int player) {
	ui_listbox_set(lobby_players_listbox, player, &player_names[player*32]);
	chat_join(player);
}

void lobby_leave(int player) {
	ui_listbox_set(lobby_players_listbox, player, "");
	chat_leave(player);
}

void lobby_ready(int player, int ready) {
	char buf[64];
	strcpy(buf, &player_names[player*32]);
	if(ready)
		strcat(buf, " (Ready)");
	ui_listbox_set(lobby_players_listbox, player, buf);
}

void lobby_progress(int player, int progress) {
	char buf[64];
	sprintf(buf, "%s (%i%%)", &player_names[player*32], progress);
	ui_listbox_set(lobby_players_listbox, player, buf);
}

void lobby_players_checkbox_ready_toggle(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	v=widget->get_prop(widget, UI_CHECKBOX_PROP_ACTIVATED);
	client_message_send(player_id, MSG_SEND_READY, v.i, 100, NULL);
}

void lobby_set_map_progress(int progress) {
	UI_PROPERTY_VALUE v={.i=progress};
	lobby_download_progress->set_prop(lobby_download_progress, UI_PROGRESSBAR_PROP_PROGRESS, v);
}
