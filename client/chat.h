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

#ifndef CHAT_H
#define CHAT_H

struct UI_PANE_LIST panelist_chat;
UI_WIDGET *chat_listbox;
UI_WIDGET *chat_hbox;
UI_WIDGET *chat_entry;
UI_WIDGET *chat_button_send;
struct UI_PANE_LIST panelist_chat_indicator;
UI_WIDGET *chat_indicator_image;

void chat_init();
void chat_button_send_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
int chat_is_visible(struct UI_PANE_LIST *panelist);
void chat_show(struct UI_PANE_LIST *panelist);
void chat_hide(struct UI_PANE_LIST *panelist);
void chat_toggle(struct UI_PANE_LIST *panelist);

void chat_indicator_show(struct UI_PANE_LIST *panelist);
void chat_indicator_hide(struct UI_PANE_LIST *panelist);
void chat_indicator_image_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void chat_recv(int player_id, char *buf, int len);
void chat_join(int player_id);
void chat_leave(int player_id);
void chat_defeated(int player_id);
void chat_countdown(int countdown);
void chat_clear();

#endif
