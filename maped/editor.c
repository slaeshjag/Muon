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
#include "editor.h"

static const char *topbar_button_text[EDITOR_TOPBAR_BUTTONS]={
	"Menu",
	"Terrain",
	"Buildings",
	"Properties"
};

static const char *player_text[MAX_PLAYERS+1]={
	"[Neutral]",
	"Player 0",
	"Player 1",
	"Player 2",
	"Player 3",
};

static const char *building_text[]={
	"Generator",
	"Scout",
	"Attacker",
	"Pylon",
	"Wall",
	"Conyard",
	"Missile silo",
	"Radar",
};

void editor_init() {
	int i;
	editor.topbar.pane=ui_pane_create(0, 0, platform.screen_w, 32, ui_widget_create_hbox());
	//editor.topbar.pane->background_color.r=editor.topbar.pane->background_color.g=editor.topbar.pane->background_color.b=0xCD;

	for(i=0; i<EDITOR_TOPBAR_BUTTONS; i++) {
		editor.topbar.button[i]=ui_widget_create_button_text(font_std, topbar_button_text[i]);
		editor.topbar.button[i]->event_handler->add(editor.topbar.button[i], editor_topbar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		ui_vbox_add_child(editor.topbar.pane->root_widget, editor.topbar.button[i], 0);
		if(i==0)
			ui_vbox_add_child(editor.topbar.pane->root_widget, ui_widget_create_spacer(), 1);
	}
	
	editor.sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 32, SIDEBAR_WIDTH, platform.screen_h-32, ui_widget_create_vbox());
	//editor.sidebar.pane->background_color.r=editor.sidebar.pane->background_color.g=editor.sidebar.pane->background_color.b=0xCD;
	
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LABEL]=ui_widget_create_label(font_std, "Buildings");
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LABEL_PLAYER]=ui_widget_create_label(font_std, "Player");
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER]=ui_widget_create_listbox(font_std);
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LABEL_BUILDING]=ui_widget_create_label(font_std, "Building");
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]=ui_widget_create_listbox(font_std);
	for(i=0; i<MAX_PLAYERS+1; i++)
		ui_listbox_add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER], player_text[i]);
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER]->event_handler->add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER], editor_sidebar_buildings_listbox_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	
	state[STATE_EDITOR].panelist=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_EDITOR].panelist->next=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_EDITOR].panelist->pane=editor.topbar.pane;
	state[STATE_EDITOR].panelist->next->pane=editor.sidebar.pane;
	state[STATE_EDITOR].panelist->next->next=NULL;
}

void editor_topbar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	ui_widget_destroy(editor.sidebar.pane->root_widget);
	ui_pane_set_root_widget(editor.sidebar.pane, ui_widget_create_vbox());
	if(widget==editor.topbar.button[EDITOR_TOPBAR_BUTTON_BUILDINGS]) {
		for(i=0; i< EDITOR_SIDEBAR_BUILDINGS_WIDGETS; i++)
			ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.buildings[i], i&&!(i&1));
	}
}

void editor_sidebar_buildings_listbox_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	UI_PROPERTY_VALUE v;
	v=widget->get_prop(widget, UI_LISTBOX_PROP_SELECTED);
	if(v.i==-1)
		return;
	ui_listbox_clear(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]);
	if(v.i==0)
		ui_listbox_add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], "Control point");
	else
		for(i=0; i<BUILDINGS; i++)
			ui_listbox_add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], building_text[i]);
}
