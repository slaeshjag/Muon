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

#ifndef GAMEOVER_H
#define GAMEOVER_H

struct UI_PANE_LIST panelist_gameover_sidebar;
struct UI_PANE_LIST panelist_gameover_stats;

UI_WIDGET *gameover_sidebar_button_menu;
UI_WIDGET *gameover_stats_label_time;
char gameover_stats_label_time_text[32];
UI_WIDGET *gameover_stats_hbox;
UI_WIDGET *gameover_stats_vbox_stat[6];

struct GAMEOVER_STATLABEL {
	UI_WIDGET *built;
	char built_text[8];
	UI_WIDGET *lost;
	char lost_text[8];
	UI_WIDGET *destroyed;
	char destroyed_text[8];
	UI_WIDGET *efficiency;
	char efficiency_text[8];
	UI_WIDGET *score;
	char score_text[8];
	UI_WIDGET *name;
} *gameover_statlabel;

void gameover_init();
void gameover_update_stats();
void gameover_sidebar_button_menu_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
