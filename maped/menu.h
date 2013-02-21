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

#ifndef MAPED_MENU_H
#define MAPED_MENU_H

enum {
	MENU_BUTTON_NEW,
	MENU_BUTTON_LOAD,
	MENU_BUTTON_QUIT,
	MENU_BUTTONS,
};

enum {
	LOAD_BUTTON_CANCEL,
	LOAD_BUTTON_LOAD,
	LOAD_BUTTONS,
};

UI_WIDGET *menu_button[MENU_BUTTONS];

struct {
	UI_WIDGET *label;
	UI_WIDGET *listbox;
	UI_WIDGET *hbox;
	UI_WIDGET *button[LOAD_BUTTONS];
} load;

void menu_init();
void load_maplist_reload();
void menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void load_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
