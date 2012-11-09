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
#include "lobby.h"

void lobby_init() {
	UI_PROPERTY_VALUE v;
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
}

void lobby_ready_checkbox_toggle(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	v=widget->get_prop(widget, UI_CHECKBOX_PROP_ACTIVATED);
	client_message_send(player_id, MSG_SEND_READY, v.i, 100, NULL);
}

void lobby_set_map_progress(int progress) {
	UI_PROPERTY_VALUE v={.i=progress};
	ui_progressbar_set_prop(lobby_progress_map, UI_PROGRESSBAR_PROP_PROGRESS, v);
}

void lobby_ready_checkbox_enable() {
	UI_PROPERTY_VALUE v={.i=0};
	lobby_checkbox_ready->set_prop(lobby_checkbox_ready, UI_CHECKBOX_PROP_ACTIVATED, v);
	lobby_checkbox_ready->event_handler->add(lobby_checkbox_ready, lobby_ready_checkbox_toggle, UI_EVENT_TYPE_UI);
}

void lobby_ready_checkbox_disable() {
	UI_PROPERTY_VALUE v={.i=0};
	lobby_checkbox_ready->set_prop(lobby_checkbox_ready, UI_CHECKBOX_PROP_ACTIVATED, v);
	lobby_checkbox_ready->event_handler->remove(lobby_checkbox_ready, lobby_ready_checkbox_toggle, UI_EVENT_TYPE_UI);
}
