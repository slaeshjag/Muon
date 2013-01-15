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

#ifndef MENU_H
#define MENU_H

//Main menu
struct UI_PANE_LIST panelist_menu_sidebar;
UI_WIDGET *menu_sidebar_button[8];
UI_WIDGET *menu_sidebar_spacer;
UI_WIDGET *menu_sidebar_button_quit;

void menu_init();

void menu_sidebar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void menu_sidebar_button_quit_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

//In-game menu
struct UI_PANE_LIST panelist_game_menu;
UI_WIDGET *game_menu_button[4];

void menu_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
