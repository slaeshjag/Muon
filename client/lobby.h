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

struct UI_PANE_LIST panelist_lobby_players, panelist_lobby_download, panelist_lobby_map;
UI_WIDGET *lobby_players_listbox;
UI_WIDGET *lobby_players_hbox;
UI_WIDGET *lobby_players_slider_team;
UI_WIDGET *lobby_players_checkbox_ready;
UI_WIDGET *lobby_players_button_kick;
UI_WIDGET *lobby_download_progress;
UI_WIDGET *lobby_map_label;
UI_WIDGET *lobby_map_imageview;
UI_WIDGET *lobby_map_button_back;

void lobby_init();

void lobby_open();
void lobby_map_preview_generate();
void lobby_join(int player_id);
void lobby_leave(int player_id);
void lobby_update_player(int player_id);
void lobby_progress(int player_id, int progress);
void lobby_download_complete();

void lobby_players_slider_team_changed(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void lobby_players_button_kick_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void lobby_players_checkbox_ready_toggle(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void lobby_map_button_back_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void lobby_set_map_progress(int progress);

#endif
