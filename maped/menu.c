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

#include <string.h>

#include "maped.h"
#include "menu.h"
#include "editor.h"
#include "map.h"

static const char *button_text[MENU_BUTTONS]={
	"New map",
	"Load map",
	"Quit"
};

void menu_init() {
	int i;
	UI_PROPERTY_VALUE v;
	state[STATE_MENU].panelist=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_MENU].panelist->next=NULL;
	state[STATE_MENU].panelist->pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-64, 256, 128, ui_widget_create_vbox());
	//state[STATE_MENU].panelist->pane->background_color.r=state[STATE_MENU].panelist->pane->background_color.g=state[STATE_MENU].panelist->pane->background_color.b=0xCD;

	for(i=0; i<MENU_BUTTONS; i++) {
		menu_button[i]=ui_widget_create_button_text(font_std, button_text[i]);
		menu_button[i]->event_handler->add(menu_button[i], menu_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		ui_vbox_add_child(state[STATE_MENU].panelist->pane->root_widget, menu_button[i], 0);
	}
	
	state[STATE_NEW].panelist=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_NEW].panelist->next=NULL;
	state[STATE_NEW].panelist->pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-80, 256, 160, ui_widget_create_vbox());
	ui_vbox_add_child(state[STATE_NEW].panelist->pane->root_widget, new.label=ui_widget_create_label(font_std, "New map"), 0);
	ui_vbox_add_child(state[STATE_NEW].panelist->pane->root_widget, new.label_w=ui_widget_create_label(font_std, "Width"), 0);
	ui_vbox_add_child(state[STATE_NEW].panelist->pane->root_widget, new.entry_w=ui_widget_create_entry(font_std), 0);
	ui_vbox_add_child(state[STATE_NEW].panelist->pane->root_widget, new.label_h=ui_widget_create_label(font_std, "Height"), 0);
	ui_vbox_add_child(state[STATE_NEW].panelist->pane->root_widget, new.entry_h=ui_widget_create_entry(font_std), 0);
	v.p="32";
	new.entry_w->set_prop(new.entry_w, UI_ENTRY_PROP_TEXT, v);
	new.entry_h->set_prop(new.entry_h, UI_ENTRY_PROP_TEXT, v);
	new.hbox=ui_widget_create_hbox();
	new.button[NEW_BUTTON_CANCEL]=ui_widget_create_button_text(font_std, "Cancel");
	new.button[NEW_BUTTON_NEW]=ui_widget_create_button_text(font_std, "New");
	ui_hbox_add_child(new.hbox, ui_widget_create_spacer(), 1);
	for(i=0; i<NEW_BUTTONS; i++) {
		ui_hbox_add_child(new.hbox, new.button[i], 0);
		new.button[i]->event_handler->add(new.button[i], new_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	}
	ui_vbox_add_child(state[STATE_NEW].panelist->pane->root_widget, new.hbox, 0);
	
	state[STATE_LOAD].panelist=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_LOAD].panelist->next=NULL;
	state[STATE_LOAD].panelist->pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-128, 256, 256, ui_widget_create_vbox());
	
	ui_vbox_add_child(state[STATE_LOAD].panelist->pane->root_widget, load.label=ui_widget_create_label(font_std, "Load map"), 0);
	ui_vbox_add_child(state[STATE_LOAD].panelist->pane->root_widget, load.listbox=ui_widget_create_listbox(font_std), 1);
	load.hbox=ui_widget_create_hbox();
	load.button[LOAD_BUTTON_CANCEL]=ui_widget_create_button_text(font_std, "Cancel");
	load.button[LOAD_BUTTON_LOAD]=ui_widget_create_button_text(font_std, "Load");
	ui_hbox_add_child(load.hbox, ui_widget_create_spacer(), 1);
	for(i=0; i<LOAD_BUTTONS; i++) {
		ui_hbox_add_child(load.hbox, load.button[i], 0);
		load.button[i]->event_handler->add(load.button[i], load_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	}
	ui_vbox_add_child(state[STATE_LOAD].panelist->pane->root_widget, load.hbox, 0);
}

void load_maplist_reload() {
	DARNIT_FILE *f;
	DARNIT_DIR_LIST *dir, *d;
	char buf[256];
	int dirs;
	ui_listbox_clear(load.listbox);
	dir=d_file_list(mapdir, DARNIT_FILESYSTEM_TYPE_READ|DARNIT_FILESYSTEM_TYPE_WRITE, &dirs);
	for(d=dir; d; d=d->next) {
		if(d->file&&ui_listbox_index_of(load.listbox, d->fname)==-1) {
			sprintf(buf, "%s/%s", mapdir, d->fname);
			f=d_file_open(buf, "rb");
			d_file_read(buf, 8, f);
			d_file_close(f);
			if(memcmp(buf, ldimagic, 8))
				continue;
			ui_listbox_add(load.listbox, d->fname);
		}
	}
	d_file_list_free(dir);
}

void menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	for(i=0; widget!=menu_button[i]; i++);
	switch(i) {
		case MENU_BUTTON_NEW:
			state_set(STATE_NEW);
			break;
		case MENU_BUTTON_LOAD:
			load_maplist_reload();
			state_set(STATE_LOAD);
			break;
		case MENU_BUTTON_QUIT:
			state_set(STATE_QUIT);
			break;
	}
}

void new_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget==new.button[NEW_BUTTON_NEW]) {
		int w, h;
		w=atoi((new.entry_w->get_prop(new.entry_w, UI_ENTRY_PROP_TEXT)).p);
		h=atoi((new.entry_h->get_prop(new.entry_h, UI_ENTRY_PROP_TEXT)).p);
		if(w<=0||h<=0)
			return;
		map=map_new(w, h, 1, d_render_tilesheet_load("res/default.png", 32, 32, DARNIT_PFORMAT_RGB5A1));
		editor_reload();
		state_set(STATE_EDITOR);
		return;
	}
	state_set(STATE_MENU);
}

void load_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	char buf[256];
	if(widget==load.button[LOAD_BUTTON_LOAD]) {
		v=load.listbox->get_prop(load.listbox, UI_LISTBOX_PROP_SELECTED);
		if(v.i<0)
			return;
		sprintf(buf, "%s/%s", mapdir, ui_listbox_get(load.listbox, v.i));
		printf("%s\n", buf);
		if((map=map_load(buf))) {
			if((v.p=(void *) map_prop_get(map, "name")))
				editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_NAME]->set_prop(editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_NAME], UI_ENTRY_PROP_TEXT, v);
			if((v.p=(void *) map_prop_get(map, "version")))
				editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_VERSION]->set_prop(editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_VERSION], UI_ENTRY_PROP_TEXT, v);
			if((v.p=(void *) map_prop_get(map, "author")))
				editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_AUTHOR]->set_prop(editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_AUTHOR], UI_ENTRY_PROP_TEXT, v);
			if((v.p=(void *) map_prop_get(map, "max_players")))
				if((v.i=atoi(v.p)-1)>=0)
					editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_SLIDER_PLAYERS]->set_prop(editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_SLIDER_PLAYERS], UI_SLIDER_PROP_VALUE, v);
			editor_reload();
			state_set(STATE_EDITOR);
			return;
		}
	}
	state_set(STATE_MENU);
}
