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

#include "maped.h"
#include "menu.h"

static const char *button_text[MENU_BUTTONS]={
	"New map",
	"Load map",
	"Quit"
};

void menu_init() {
	int i;
	state[STATE_MENU].panelist=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_MENU].panelist->next=NULL;
	state[STATE_MENU].panelist->pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-64, 256, 128, ui_widget_create_vbox());
	//state[STATE_MENU].panelist->pane->background_color.r=state[STATE_MENU].panelist->pane->background_color.g=state[STATE_MENU].panelist->pane->background_color.b=0xCD;

	for(i=0; i<MENU_BUTTONS; i++) {
		menu_button[i]=ui_widget_create_button_text(font_std, button_text[i]);
		menu_button[i]->event_handler->add(menu_button[i], menu_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		ui_vbox_add_child(state[STATE_MENU].panelist->pane->root_widget, menu_button[i], 0);
	}
}

void menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	for(i=0; widget!=menu_button[i]; i++);
	switch(i) {
		case MENU_BUTTON_NEW:
			state_set(STATE_EDITOR);
			break;
		case MENU_BUTTON_QUIT:
			state_set(STATE_QUIT);
			break;
	}
}
