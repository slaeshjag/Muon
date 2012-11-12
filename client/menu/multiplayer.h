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

#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

struct UI_PANE_LIST panelist_multiplayer_host;
UI_WIDGET *multiplayer_host_vbox;
UI_WIDGET *multiplayer_host_listbox_maps;
UI_WIDGET *multiplayer_host_entry_port;
UI_WIDGET *multiplayer_host_slider_players;
UI_WIDGET *multiplayer_host_slider_gamespeed;
UI_WIDGET *multiplayer_host_button_host;

struct UI_PANE_LIST panelist_multiplayer_join;
UI_WIDGET *multiplayer_join_entry_host;
UI_WIDGET *multiplayer_join_entry_port;
UI_WIDGET *multiplayer_join_button;

struct UI_PANE_LIST panelist_multiplayer_connecting;
UI_WIDGET *multiplayer_connecting_button_cancel;

void menu_multiplayer_init();
void menu_multiplayer_host_maps_reload();

void multiplayer_host_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void multiplayer_join_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void multiplayer_connecting_button_cancel_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
