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
#include "view.h"
#include "game.h"
#include "chat.h"

void chat_init() {
	panelist_chat.pane=ui_pane_create(16, platform.screen_h-256-16, 200, 256, NULL);
	panelist_chat.next=NULL;
	ui_pane_set_root_widget(panelist_chat.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_chat.pane->root_widget, ui_widget_create_label(font_std, T("Chat")), 0);
	chat_listbox=ui_widget_create_listbox(font_std);
	ui_vbox_add_child(panelist_chat.pane->root_widget, chat_listbox, 1);
	chat_hbox_team=ui_widget_create_hbox();
	chat_checkbox_team=ui_widget_create_checkbox();
	ui_hbox_add_child(chat_hbox_team, chat_checkbox_team, 0);
	ui_hbox_add_child(chat_hbox_team, ui_widget_create_label(font_std, T("Send to team")), 0);
	ui_vbox_add_child(panelist_chat.pane->root_widget, chat_hbox_team, 0);
	chat_hbox=ui_widget_create_hbox();
	chat_button_send=ui_widget_create_button_text(font_std, T("Send"));
	chat_entry=ui_widget_create_entry(font_std);
	ui_hbox_add_child(chat_hbox, chat_entry, 1);
	ui_hbox_add_child(chat_hbox, chat_button_send, 0);
	ui_vbox_add_child(panelist_chat.pane->root_widget, chat_hbox, 0);
	chat_button_send->event_handler->add(chat_button_send, chat_button_send_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	chat_entry->event_handler->add(chat_entry, chat_button_send_click, UI_EVENT_TYPE_KEYBOARD_PRESS);
	
	panelist_chat_indicator.pane=ui_pane_create(0, platform.screen_h-(32+UI_PADDING*2), 32+UI_PADDING*2, 32+UI_PADDING*2, NULL);
	panelist_chat_indicator.next=NULL;
	chat_indicator_image=ui_widget_create_imageview_file("res/chat.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	ui_pane_set_root_widget(panelist_chat_indicator.pane, chat_indicator_image);
	chat_indicator_image->event_handler->add(chat_indicator_image, chat_indicator_image_click, UI_EVENT_TYPE_MOUSE_PRESS);
}

int chat_is_visible(struct UI_PANE_LIST *panelist) {
	struct UI_PANE_LIST *p;
	for(p=panelist; p; p=p->next)
		if(p==&panelist_chat)
			return 1;
	return 0;
}

void chat_show(struct UI_PANE_LIST *panelist) {
	if(!panelist)
		return;
	chat_indicator_hide(panelist);
	struct UI_PANE_LIST *p;
	for(p=panelist; p->next; p=p->next) {
		if(p->next==&panelist_chat||p==&panelist_chat) {
			ui_selected_widget=chat_entry;
			return;
		}
	}
	p->next=&panelist_chat;
	ui_selected_widget=chat_entry;
}

void chat_hide(struct UI_PANE_LIST *panelist) {
	if(!panelist)
		return;
	struct UI_PANE_LIST *p;
	for(p=panelist; p->next; p=p->next) {
		if(p->next==&panelist_chat) {
			p->next=panelist_chat.next;
			ui_selected_widget=NULL;
			return;
		}
	}
}

void chat_toggle(struct UI_PANE_LIST *panelist) {
	struct UI_PANE_LIST *p;
	for(p=panelist; p; p=p->next) {
		if(p->next==&panelist_chat) {
			p->next=panelist_chat.next;
			ui_selected_widget=NULL;
			return;
		}
		if(p->next==NULL) {
			chat_show(panelist);
			ui_selected_widget=chat_entry;
			return;
		}
	}
}

void chat_indicator_show(struct UI_PANE_LIST *panelist) {
	if(!panelist)
		return;
	struct UI_PANE_LIST *p;
	for(p=panelist; p->next; p=p->next) {
		if(p->next==&panelist_chat_indicator||p==&panelist_chat_indicator) {
			return;
		}
	}
	p->next=&panelist_chat_indicator;
}

void chat_indicator_hide(struct UI_PANE_LIST *panelist) {
	if(!panelist)
		return;
	struct UI_PANE_LIST *p;
	for(p=panelist; p->next; p=p->next) {
		if(p->next==&panelist_chat_indicator) {
			p->next=panelist_chat_indicator.next;
			return;
		}
	}
}

void chat_indicator_image_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	chat_indicator_hide(&panelist_game_sidebar);
	chat_show(&panelist_game_sidebar);
}

void chat_button_send_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if((type==UI_EVENT_TYPE_KEYBOARD_PRESS&&e->keyboard->keysym!=KEY(RETURN)&&e->keyboard->keysym!=KEY(KP_ENTER)))
		return;
	UI_PROPERTY_VALUE v;
	v=chat_checkbox_team->get_prop(chat_checkbox_team, UI_CHECKBOX_PROP_ACTIVATED);
	unsigned int team=v.i?player[player_id].team:0;
	v=chat_entry->get_prop(chat_entry, UI_ENTRY_PROP_TEXT);
	if(strlen(v.p)==0)
		return;
	client_message_send(player_id, MSG_SEND_CHAT, team, strlen(v.p), v.p);
	v.p="";
	chat_entry->set_prop(chat_entry, UI_ENTRY_PROP_TEXT, v);
}

void chat_recv(int player_id, unsigned int team, char *buf, int len) {
	//buf must be at least one byte larger than len!
	char *chatmsg;
	if(!(chatmsg=malloc(len+36)))
		return;
	buf[len]=0;
	char *format=team?"[%s] %s\n":"<%s> %s\n";
	sprintf(chatmsg, format, player[player_id].name, buf);
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
	if(gamestate==GAME_STATE_GAME&&!chat_is_visible(&panelist_game_sidebar))
		chat_indicator_show(&panelist_game_sidebar);
}

void chat_join(int player_id) {
	char *chatmsg;
	if(!(chatmsg=malloc(64)))
		return;
	sprintf(chatmsg, " * %s %s", player[player_id].name, T("joined"));
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
}

void chat_leave(int player_id) {
	char *chatmsg;
	if(!(chatmsg=malloc(64)))
		return;
	sprintf(chatmsg, " * %s %s", player[player_id].name, T("disconnected"));
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
}

void chat_defeated(int player_id) {
	char *chatmsg;
	if(!(chatmsg=malloc(64)))
		return;
	sprintf(chatmsg, " * %s %s", player[player_id].name, T("defeated"));
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
}

void chat_countdown(int countdown) {
	char *chatmsg;
	if(!(chatmsg=malloc(32)))
		return;
	sprintf(chatmsg, " * %s %i", T("Game starts in"), countdown);
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
}

void chat_clear() {
	ui_listbox_clear(chat_listbox);
}
