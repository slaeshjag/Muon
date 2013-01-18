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
#include "../chat.h"
#include "../game.h"
#include "../map.h"
#include "menu.h"
#include "multiplayer.h"
#include "settings.h"

const char **menu_sidebar_button_text_main;

void menu_init() {
	int i;
	
	menu_multiplayer_init();
	menu_settings_init();
	
	//Main menu
	panelist_menu_sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, platform.screen_h, NULL);
	panelist_menu_sidebar.next=NULL;
	ui_pane_set_root_widget(panelist_menu_sidebar.pane, ui_widget_create_vbox());
	
	//TODO: fixfixfix when making proper menues, this is damned ugly
	menu_sidebar_button_text_main=calloc(sizeof(void *), 8);
	menu_sidebar_button_text_main[0]=T("Singleplayer");
	menu_sidebar_button_text_main[1]=T("Multiplayer");
	menu_sidebar_button_text_main[2]=T("Settings");
	
	for(i=0; i<8; i++) {
		if(!menu_sidebar_button_text_main[i])
			continue;
		menu_sidebar_button[i]=ui_widget_create_button(ui_widget_create_label(font_std, menu_sidebar_button_text_main[i]));
		menu_sidebar_button[i]->event_handler->add(menu_sidebar_button[i], menu_sidebar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, menu_sidebar_button[i], 0);
	}
	ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, ui_widget_create_spacer(), 1);
	menu_sidebar_button_quit=ui_widget_create_button(ui_widget_create_label(font_std, T("Quit game")));
	menu_sidebar_button_quit->event_handler->add(menu_sidebar_button_quit, menu_sidebar_button_quit_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, menu_sidebar_button_quit, 0);
	
	//In-game menu
	panelist_game_menu.pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-128, 256, 256, NULL);
	ui_pane_set_root_widget(panelist_game_menu.pane, ui_widget_create_vbox());
	panelist_game_menu.next=&panelist_chat;
	ui_vbox_add_child(panelist_game_menu.pane->root_widget, ui_widget_create_label(font_std, "Muon\n===="), 0);
	game_menu_button[0]=ui_widget_create_button(ui_widget_create_label(font_std, T("Disconnect")));
	game_menu_button[1]=ui_widget_create_button(ui_widget_create_label(font_std, T("Resign")));
	game_menu_button[2]=ui_widget_create_button(ui_widget_create_label(font_std, T("Quit game")));
	game_menu_button[3]=ui_widget_create_button(ui_widget_create_label(font_std, T("Return to game")));
	for(i=0; i<3; i++) {
		ui_vbox_add_child(panelist_game_menu.pane->root_widget, game_menu_button[i], 0);
		game_menu_button[i]->event_handler->add(game_menu_button[i], game_menu_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	}
	ui_vbox_add_child(panelist_game_menu.pane->root_widget, ui_widget_create_spacer(), 1);
	ui_vbox_add_child(panelist_game_menu.pane->root_widget, game_menu_button[3], 0);
	game_menu_button[3]->event_handler->add(game_menu_button[3], game_menu_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
}

//Main menu
void menu_sidebar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	/*int i;
	for(i=0; i<8; i++) {
		if(!menu_sidebar_button_text_main[i])
			continue;
		//menu_sidebar_button[i]=ui_widget_create_button(ui_widget_create_label(font_std, menu_sidebar_button_text_main[i]));
		//menu_sidebar_button[i]->event_handler->add(menu_sidebar_button[i], menu_sidebar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		ui_vbox_remove_child(panelist_menu_sidebar.pane->root_widget, menu_sidebar_button[i]);
	}*/
	if(widget==menu_sidebar_button[1]) {
		panelist_menu_sidebar.next=&panelist_multiplayer_host;
		ui_selected_widget=multiplayer_join_entry_host;
		ui_selected_widget=NULL;
	} else if(widget==menu_sidebar_button[2]) {
		//panelist_menu_sidebar.next=&panelist_input_name;
		panelist_menu_sidebar.next=&panelist_settings_monitor;
		ui_selected_widget=settings_game_entry_name;
	} else {
		panelist_menu_sidebar.next=NULL;
		ui_selected_widget=NULL;
	}
}

void menu_sidebar_button_quit_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	game_state(GAME_STATE_QUIT);
}

void menu_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(e->buttons->start&&!prevbuttons.start)
		game_state(gamestate==GAME_STATE_MENU?GAME_STATE_QUIT:GAME_STATE_GAME);
	
	memcpy(&prevbuttons, e->buttons, sizeof(UI_EVENT_BUTTONS));
}

//In-game menu
void game_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget==game_menu_button[0]) {
		client_disconnect(-1);
		game_state(GAME_STATE_MENU);
		panelist_menu_sidebar.next=NULL;
	} else if(widget==game_menu_button[1]) {
		client_message_send(player_id, MSG_SEND_PLACE_BUILDING, BUILDING_NONE, map_get_home(), NULL);
		game_state(GAME_STATE_GAME);
		chat_show(&panelist_game_sidebar);
	} else if(widget==game_menu_button[2]) {
		game_state(GAME_STATE_QUIT);
	} else if(widget==game_menu_button[3]) {
		game_state(GAME_STATE_GAME);
	}
}
