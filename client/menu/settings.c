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

#include "../muon.h"
#include "../client.h"
#include "menu.h"
#include "settings.h"

void menu_settings_init() {
	int i;
	UI_PROPERTY_VALUE v={.p=NULL};
	
	//Gameplay settings
	panelist_settings_game.pane=ui_pane_create(16, 32+300, 256, 128, NULL);
	ui_pane_set_root_widget(panelist_settings_game.pane, ui_widget_create_vbox());
	panelist_settings_game.next=NULL;
	ui_vbox_add_child(panelist_settings_game.pane->root_widget, ui_widget_create_label(font_std, T("Player name")), 1);
	settings_game_entry_name=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_settings_game.pane->root_widget, settings_game_entry_name, 0);
	settings_game_entry_name->event_handler->add(settings_game_entry_name, settings_game_button_ok_click, UI_EVENT_TYPE_KEYBOARD_PRESS);
	v.p=config.player_name;
	settings_game_entry_name->set_prop(settings_game_entry_name, UI_ENTRY_PROP_TEXT, v);
	
	settings_game_hbox_grid=ui_widget_create_hbox();
	settings_game_checkbox_grid=ui_widget_create_checkbox();
	v.i=config.grid;
	settings_game_checkbox_grid->set_prop(settings_game_checkbox_grid, UI_CHECKBOX_PROP_ACTIVATED, v);
	ui_hbox_add_child(settings_game_hbox_grid, settings_game_checkbox_grid, 0);
	ui_hbox_add_child(settings_game_hbox_grid, ui_widget_create_label(font_std, T("Show grid")), 0);
	ui_vbox_add_child(panelist_settings_game.pane->root_widget, settings_game_hbox_grid, 0);
	
	settings_game_hbox_powergrid=ui_widget_create_hbox();
	settings_game_checkbox_powergrid=ui_widget_create_checkbox();
	v.i=config.powergrid;
	settings_game_checkbox_powergrid->set_prop(settings_game_checkbox_powergrid, UI_CHECKBOX_PROP_ACTIVATED, v);
	ui_hbox_add_child(settings_game_hbox_powergrid, settings_game_checkbox_powergrid, 0);
	ui_hbox_add_child(settings_game_hbox_powergrid, ui_widget_create_label(font_std, T("Always show power grid")), 0);
	ui_vbox_add_child(panelist_settings_game.pane->root_widget, settings_game_hbox_powergrid, 0);
	
	settings_game_button_ok=ui_widget_create_button_text(font_std, T("OK"));
	ui_vbox_add_child(panelist_settings_game.pane->root_widget, settings_game_button_ok, 0);
	settings_game_button_ok->event_handler->add(settings_game_button_ok, settings_game_button_ok_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	
	//Monitor settings
	panelist_settings_monitor.pane=ui_pane_create(16, 16, 320, 300, NULL);
	panelist_settings_monitor.next=&panelist_settings_game;
	ui_pane_set_root_widget(panelist_settings_monitor.pane, ui_widget_create_hbox());
	settings_monitor_listbox_modes=ui_widget_create_listbox(font_std);
	settings_monitor_listbox_modes->event_handler->add(settings_monitor_listbox_modes, settings_monitor_listbox_modes_changed, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_hbox_add_child(panelist_settings_monitor.pane->root_widget, settings_monitor_listbox_modes, 0);
	settings_monitor_vbox=ui_widget_create_vbox();
	ui_hbox_add_child(panelist_settings_monitor.pane->root_widget, settings_monitor_vbox, 1);
	settings_monitor_entry_w=ui_widget_create_entry(font_std);
	settings_monitor_entry_h=ui_widget_create_entry(font_std);
	ui_vbox_add_child(settings_monitor_vbox, ui_widget_create_label(font_std, T("Screen width")), 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_entry_w, 0);
	ui_vbox_add_child(settings_monitor_vbox, ui_widget_create_label(font_std, T("Screen height")), 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_entry_h, 0);
	settings_monitor_hbox_fullscreen=ui_widget_create_hbox();
	settings_monitor_checkbox_fullscreen=ui_widget_create_checkbox();
	ui_hbox_add_child(settings_monitor_hbox_fullscreen, settings_monitor_checkbox_fullscreen, 0);
	ui_hbox_add_child(settings_monitor_hbox_fullscreen, ui_widget_create_label(font_std, T("Fullscreen")), 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_hbox_fullscreen, 0);
	ui_vbox_add_child(settings_monitor_vbox, ui_widget_create_label(font_std, T("Plasma scale")), 0);
	settings_monitor_slider_plasma=ui_widget_create_slider(6);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_slider_plasma, 0);
	settings_monitor_hbox_alpha=ui_widget_create_hbox();
	settings_monitor_checkbox_alpha=ui_widget_create_checkbox();
	ui_hbox_add_child(settings_monitor_hbox_alpha, settings_monitor_checkbox_alpha, 0);
	ui_hbox_add_child(settings_monitor_hbox_alpha, ui_widget_create_label(font_std, T("Alpha blending")), 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_hbox_alpha, 0);
	ui_vbox_add_child(settings_monitor_vbox, ui_widget_create_spacer(), 1);
	settings_monitor_button_ok=ui_widget_create_button_text(font_std, T("OK"));
	settings_monitor_button_ok->event_handler->add(settings_monitor_button_ok, settings_monitor_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	settings_monitor_button_cancel=ui_widget_create_button_text(font_std, T("Cancel"));
	settings_monitor_button_cancel->event_handler->add(settings_monitor_button_cancel, settings_monitor_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_button_ok, 0);
	ui_vbox_add_child(settings_monitor_vbox, settings_monitor_button_cancel, 0);
	char buf[32];
	v.p=buf;
	sprintf(buf, "%i", config.screen_w);
	settings_monitor_entry_w->set_prop(settings_monitor_entry_w, UI_ENTRY_PROP_TEXT, v);
	sprintf(buf, "%i", config.screen_h);
	settings_monitor_entry_h->set_prop(settings_monitor_entry_h, UI_ENTRY_PROP_TEXT, v);
	v.i=config.fullscreen;
	settings_monitor_checkbox_fullscreen->set_prop(settings_monitor_checkbox_fullscreen, UI_CHECKBOX_PROP_ACTIVATED, v);
	v.i=config.plasma;
	settings_monitor_slider_plasma->set_prop(settings_monitor_slider_plasma, UI_SLIDER_PROP_VALUE, v);
	v.i=config.alpha;
	settings_monitor_checkbox_alpha->set_prop(settings_monitor_checkbox_alpha, UI_CHECKBOX_PROP_ACTIVATED, v);
	for(i=0; videomodes[i]; i++) {
		sprintf(buf, "%ix%i", videomodes[i]->w, videomodes[i]->h);
		ui_listbox_add(settings_monitor_listbox_modes, buf);
	}
}

void settings_game_button_ok_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if((type==UI_EVENT_TYPE_KEYBOARD_PRESS&&e->keyboard->keysym!=KEY(RETURN)))
		return;
	UI_PROPERTY_VALUE v;
	v=settings_game_entry_name->get_prop(settings_game_entry_name, UI_ENTRY_PROP_TEXT);
	if(!strlen(v.p))
		return;
	memset(config.player_name, 0, 32);
	strncpy(config.player_name, v.p, 31);
	config.player_name[31]=0;
	v=settings_game_checkbox_grid->get_prop(settings_game_checkbox_grid, UI_CHECKBOX_PROP_ACTIVATED);
	config.grid=v.i;
	v=settings_game_checkbox_powergrid->get_prop(settings_game_checkbox_powergrid, UI_CHECKBOX_PROP_ACTIVATED);
	config.powergrid=v.i;
	platform_config_write();
	//printf("Player name: %s\n", player_name);
	panelist_menu_sidebar.next=NULL;
}

void settings_monitor_listbox_modes_changed(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	char buf[32];
	UI_PROPERTY_VALUE v;
	v=widget->get_prop(widget, UI_LISTBOX_PROP_SELECTED);
	int i=v.i;
	v.p=buf;
	sprintf(buf, "%i", videomodes[i]->w);
	settings_monitor_entry_w->set_prop(settings_monitor_entry_w, UI_ENTRY_PROP_TEXT, v);
	sprintf(buf, "%i", videomodes[i]->h);
	settings_monitor_entry_h->set_prop(settings_monitor_entry_h, UI_ENTRY_PROP_TEXT, v);
}

void settings_monitor_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget==settings_monitor_button_ok) {
		UI_PROPERTY_VALUE v;
		v=settings_monitor_entry_w->get_prop(settings_monitor_entry_w, UI_ENTRY_PROP_TEXT);
		config.screen_w=atoi(v.p);
		v=settings_monitor_entry_h->get_prop(settings_monitor_entry_h, UI_ENTRY_PROP_TEXT);
		config.screen_h=atoi(v.p);
		v=settings_monitor_checkbox_fullscreen->get_prop(settings_monitor_checkbox_fullscreen, UI_CHECKBOX_PROP_ACTIVATED);
		config.fullscreen=v.i;
		v=settings_monitor_slider_plasma->get_prop(settings_monitor_slider_plasma, UI_SLIDER_PROP_VALUE);
		config.plasma=v.i;
		v=settings_monitor_checkbox_alpha->get_prop(settings_monitor_checkbox_alpha, UI_CHECKBOX_PROP_ACTIVATED);
		config.alpha=v.i;
		platform_config_write();
		ui_messagebox(font_std, T("Please restart Muon for the changes to take place."));
	}
	panelist_menu_sidebar.next=NULL;
}
