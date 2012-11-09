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

#ifndef LOBBY_H
#define LOBBY_H

struct UI_PANE_LIST panelist_lobby, panelist_lobby_ready;
UI_WIDGET *lobby_countdown_label;
UI_WIDGET *lobby_progress_map;
char lobby_countdown_text[16];
UI_WIDGET *lobby_checkbox_ready;

void lobby_init();
void lobby_ready_checkbox_toggle(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void lobby_set_map_progress(int progress);
void lobby_ready_checkbox_enable();
void lobby_ready_checkbox_disable();

#endif
