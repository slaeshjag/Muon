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
	int spacing=platform_lowres?0:16;
	//Player list
	panelist_lobby_players.pane=ui_pane_create(spacing, spacing, platform_lowres?200:256, platform_lowres?112:128+48, NULL);
	panelist_lobby_players.next=&panelist_lobby_download;
	ui_pane_set_root_widget(panelist_lobby_players.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, ui_widget_create_label(font_std, T("Players")), 0);
	lobby_players_listbox=ui_widget_create_listbox(font_std);
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, lobby_players_listbox, 1);
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, ui_widget_create_label(font_std, T("Team:")), 0);
	lobby_players_slider_team=ui_widget_create_slider(5);
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, lobby_players_slider_team, 0);
	lobby_players_slider_team->event_handler->add(lobby_players_slider_team, lobby_players_slider_team_changed, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	lobby_players_hbox=ui_widget_create_hbox();
	lobby_players_checkbox_ready=ui_widget_create_checkbox();
	lobby_players_button_kick=ui_widget_create_button_text(font_std, T("Kick"));
	lobby_players_button_kick->event_handler->add(lobby_players_button_kick, lobby_players_button_kick_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_hbox_add_child(lobby_players_hbox, lobby_players_checkbox_ready, 0);
	ui_hbox_add_child(lobby_players_hbox, ui_widget_create_label(font_std, T("Ready")), 0);
	ui_hbox_add_child(lobby_players_hbox, ui_widget_create_spacer(), 1);
	ui_hbox_add_child(lobby_players_hbox, lobby_players_button_kick, 0);
	ui_vbox_add_child(panelist_lobby_players.pane->root_widget, lobby_players_hbox, 0);
	lobby_players_checkbox_ready->event_handler->add(lobby_players_checkbox_ready, lobby_players_checkbox_ready_toggle, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	
	//Map download progress
	panelist_lobby_download.pane=ui_pane_create(platform.screen_w/2-64, platform.screen_h/2-32, 128, 64, NULL);
	panelist_lobby_download.next=&panelist_lobby_map;
	ui_pane_set_root_widget(panelist_lobby_download.pane, ui_widget_create_vbox());
	lobby_download_progress=ui_widget_create_progressbar(font_std);
	ui_vbox_add_child(panelist_lobby_download.pane->root_widget, ui_widget_create_label(font_std, T("Downloading map")), 1);
	ui_vbox_add_child(panelist_lobby_download.pane->root_widget, lobby_download_progress, 0);
	v.i=0;
	lobby_download_progress->set_prop(lobby_download_progress, UI_PROGRESSBAR_PROP_PROGRESS, v);
	
	//Sidebar with map preview
	panelist_lobby_map.pane=ui_pane_create(platform.screen_w-sidebar_width, 0, sidebar_width, platform.screen_h, NULL);
	lobby_map_imageview=ui_widget_create_imageview_raw(sidebar_width-2*ui_padding, sidebar_width-2*ui_padding, DARNIT_PFORMAT_RGB5A1);
	lobby_map_label=ui_widget_create_label(font_std, T("Map"));
	lobby_map_button_back=ui_widget_create_button_text(font_std, T("Back"));
	lobby_map_button_back->event_handler->add(lobby_map_button_back, lobby_map_button_back_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_pane_set_root_widget(panelist_lobby_map.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_lobby_map.pane->root_widget, lobby_map_label, 0);
	ui_vbox_add_child(panelist_lobby_map.pane->root_widget, lobby_map_imageview, 0);
	ui_vbox_add_child(panelist_lobby_map.pane->root_widget, ui_widget_create_spacer(), 1);
	ui_vbox_add_child(panelist_lobby_map.pane->root_widget, lobby_map_button_back, 0);
}

void lobby_open() {
	int i;
	UI_PROPERTY_VALUE v={.i=0};
	lobby_players_slider_team->set_prop(lobby_players_slider_team, UI_SLIDER_PROP_VALUE, v);
	chat_checkbox_team->set_prop(chat_checkbox_team, UI_CHECKBOX_PROP_ACTIVATED, v);
	chat_checkbox_team->enabled=0;
	lobby_players_checkbox_ready->set_prop(lobby_players_checkbox_ready, UI_CHECKBOX_PROP_ACTIVATED, v);
	lobby_players_checkbox_ready->enabled=0;
	ui_listbox_clear(lobby_players_listbox);
	for(i=0; i<players; i++)
		ui_listbox_add(lobby_players_listbox, "");
	panelist_lobby_players.next=&panelist_lobby_download;
	chat_show(gamestate_pane[GAME_STATE_LOBBY]);
	ui_selected_widget=chat_entry;
	
	v=lobby_map_imageview->get_prop(lobby_map_imageview, UI_IMAGEVIEW_PROP_TILESHEET);
	map_minimap_clear(v.p, lobby_map_imageview->w, lobby_map_imageview->h);
	
	ui_hbox_remove_child(lobby_players_hbox, lobby_players_button_kick);
	if(!server_is_running())
		ui_hbox_add_child(lobby_players_hbox, lobby_players_button_kick, 0);
}

void lobby_map_preview_generate() {
	UI_PROPERTY_VALUE v;
	v.p=(void *)d_map_prop(map->prop, "name");
	if(v.p)
		lobby_map_label->set_prop(lobby_map_label, UI_LABEL_PROP_TEXT, v);
	v=lobby_map_imageview->get_prop(lobby_map_imageview, UI_IMAGEVIEW_PROP_TILESHEET);
	map_minimap_update(v.p, lobby_map_imageview->w, lobby_map_imageview->h, 0);
}

void lobby_join(int player_id) {
	ui_listbox_set(lobby_players_listbox, player_id, player[player_id].name);
	chat_join(player_id);
}

void lobby_leave(int player_id) {
	ui_listbox_set(lobby_players_listbox, player_id, "");
	chat_leave(player_id);
}

void lobby_update_player(int player_id) {
	char buf[64];
	
	if(player[player_id].team)
		sprintf(buf, "%s [%i]", player[player_id].name, player[player_id].team);
	else
		sprintf(buf, "%s", player[player_id].name);
	
	if(player[player_id].ready) {
		strcat(buf, " (");
		strcat(buf, T("Ready"));
		strcat(buf, ")");
	}
		
	ui_listbox_set(lobby_players_listbox, player_id, buf);
}

void lobby_progress(int player_id, int progress) {
	char buf[64];
	sprintf(buf, "%s (%i%%)", player[player_id].name, progress);
	ui_listbox_set(lobby_players_listbox, player_id, buf);
}

void lobby_download_complete() {
	panelist_lobby_players.next=panelist_lobby_download.next;
	lobby_players_checkbox_ready->enabled=1;
}

void lobby_players_slider_team_changed(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	v=lobby_players_slider_team->get_prop(lobby_players_slider_team, UI_SLIDER_PROP_VALUE);
	client_message_send(player_id, MSG_SEND_PLAYER_INFO, v.i, 0, NULL);
	
	if(!v.i)
		chat_checkbox_team->set_prop(chat_checkbox_team, UI_CHECKBOX_PROP_ACTIVATED, v);
	chat_checkbox_team->enabled=v.i;
}

void lobby_players_button_kick_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	v=lobby_players_listbox->get_prop(lobby_players_listbox, UI_LISTBOX_PROP_SELECTED);
	if(v.i>=0)
		client_message_send(player_id, MSG_SEND_KICK, v.i, 0, NULL);
}

void lobby_players_checkbox_ready_toggle(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	v=widget->get_prop(widget, UI_CHECKBOX_PROP_ACTIVATED);
	client_message_send(player_id, MSG_SEND_READY, v.i, 100, NULL);
}

void lobby_map_button_back_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	client_disconnect(-1);
}

void lobby_set_map_progress(int progress) {
	UI_PROPERTY_VALUE v={.i=progress};
	lobby_download_progress->set_prop(lobby_download_progress, UI_PROGRESSBAR_PROP_PROGRESS, v);
}
