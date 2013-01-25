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
 
 #ifndef SETTINGS_H
 #define SETTINGS_H
 
struct UI_PANE_LIST panelist_settings_game;
UI_WIDGET *settings_game_entry_name;
UI_WIDGET *settings_game_hbox_grid;
UI_WIDGET *settings_game_checkbox_grid;
UI_WIDGET *settings_game_hbox_powergrid;
UI_WIDGET *settings_game_checkbox_powergrid;
UI_WIDGET *settings_game_button_ok;
 
struct UI_PANE_LIST panelist_settings_monitor;
UI_WIDGET *settings_monitor_vbox;
UI_WIDGET *settings_monitor_hbox_fullscreen;
UI_WIDGET *settings_monitor_hbox_alpha;
UI_WIDGET *settings_monitor_listbox_modes;
UI_WIDGET *settings_monitor_entry_w;
UI_WIDGET *settings_monitor_entry_h;
UI_WIDGET *settings_monitor_checkbox_fullscreen;
UI_WIDGET *settings_monitor_checkbox_alpha;
UI_WIDGET *settings_monitor_slider_plasma;
UI_WIDGET *settings_monitor_button_ok;

struct UI_PANE_LIST panelist_settings_sound;
UI_WIDGET *settings_sound_label_sound;
UI_WIDGET *settings_sound_slider_sound;
UI_WIDGET *settings_sound_label_music;
UI_WIDGET *settings_sound_slider_music;
UI_WIDGET *settings_sound_button_ok;

void menu_settings_init();

void settings_monitor_listbox_modes_changed(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void settings_monitor_button_save_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void settings_game_button_save_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void settings_sound_button_save_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
 
 #endif
