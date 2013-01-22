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

const char *menu_sidebar_button_text_main[8];

void menu_init() {
	int i;
	
	menu_multiplayer_init();
	menu_settings_init();
	
	//Main menu
	panelist_menu_sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, platform.screen_h, NULL);
	panelist_menu_sidebar.next=NULL;
	ui_pane_set_root_widget(panelist_menu_sidebar.pane, ui_widget_create_vbox());
	
	memset(&menu, 0, sizeof(menu));
	menustate=MENU_STATE_MAIN;
	
	menu[MENU_STATE_MAIN].event_handler=menu_main_button_click;
	menu[MENU_STATE_MAIN].label=ui_widget_create_label(font_std, T("Muon"));
	menu[MENU_STATE_MAIN].button[0]=ui_widget_create_button_text(font_std, T("Singleplayer"));
	menu[MENU_STATE_MAIN].button[1]=ui_widget_create_button_text(font_std, T("Multiplayer"));
	menu[MENU_STATE_MAIN].button[2]=ui_widget_create_button_text(font_std, T("Settings"));
	menu[MENU_STATE_MAIN].spacer=ui_widget_create_spacer();
	menu[MENU_STATE_MAIN].button_back=ui_widget_create_button_text(font_std, T("Quit game"));
	
	menu[MENU_STATE_MULTIPLAYER].event_handler=menu_multiplayer_button_click;
	menu[MENU_STATE_MULTIPLAYER].label=ui_widget_create_label(font_std, T("Multiplayer"));
	menu[MENU_STATE_MULTIPLAYER].button[0]=ui_widget_create_button_text(font_std, T("Host game"));
	menu[MENU_STATE_MULTIPLAYER].button[1]=ui_widget_create_button_text(font_std, T("Join game"));
	menu[MENU_STATE_MULTIPLAYER].spacer=menu[MENU_STATE_MAIN].spacer;
	menu[MENU_STATE_MULTIPLAYER].button_back=ui_widget_create_button_text(font_std, T("Back"));
	
	menu[MENU_STATE_SETTINGS].event_handler=menu_settings_button_click;
	menu[MENU_STATE_SETTINGS].label=ui_widget_create_label(font_std, T("Settings"));
	menu[MENU_STATE_SETTINGS].button[0]=ui_widget_create_button_text(font_std, T("Game"));
	menu[MENU_STATE_SETTINGS].button[1]=ui_widget_create_button_text(font_std, T("Video"));
	menu[MENU_STATE_SETTINGS].spacer=menu[MENU_STATE_MAIN].spacer;
	menu[MENU_STATE_SETTINGS].button_back=menu[MENU_STATE_MULTIPLAYER].button_back;
	
	menu[MENU_STATE_MAIN].button_back->event_handler->add(menu[MENU_STATE_MAIN].button_back, menu_sidebar_button_quit_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	menu[MENU_STATE_MULTIPLAYER].button_back->event_handler->add(menu[MENU_STATE_MULTIPLAYER].button_back, menu_sidebar_button_quit_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	enum MENU_STATE state;
	for(state=0; state<MENU_STATES; state++)
		for(i=0; i<8; i++) {
			if(!menu[state].button[i])
				continue;
			menu[state].button[i]->event_handler->add(menu[state].button[i], menu[state].event_handler, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		}
	
	menu_state(MENU_STATE_MAIN);
	
	
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

void menu_state(enum MENU_STATE state) {
	//We need to do this the next iteration to not segfault when removing widgets that are running thier eventhandler
	menu_newstate=state;
	panelist_menu_sidebar.next=NULL;
	ui_selected_widget=NULL;
	ui_event_global_add(meni_state_change, UI_EVENT_TYPE_MOUSE_ENTER);
}

void meni_state_change(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	ui_vbox_remove_child(panelist_menu_sidebar.pane->root_widget, menu[menustate].label);
	for(i=0; i<8; i++) {
		if(!menu[menustate].button[i])
			continue;
		ui_vbox_remove_child(panelist_menu_sidebar.pane->root_widget, menu[menustate].button[i]);
	}
	ui_vbox_remove_child(panelist_menu_sidebar.pane->root_widget, menu[menustate].spacer);
	ui_vbox_remove_child(panelist_menu_sidebar.pane->root_widget, menu[menustate].button_back);
	
	menustate=menu_newstate;
	
	ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, menu[menustate].label, 0);
	for(i=0; i<8; i++) {
		if(!menu[menustate].button[i])
			continue;
		ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, menu[menustate].button[i], 0);
	}
	ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, menu[menustate].spacer, 1);
	ui_vbox_add_child(panelist_menu_sidebar.pane->root_widget, menu[menustate].button_back, 0);
	
	ui_event_global_remove(meni_state_change, UI_EVENT_TYPE_UI_EVENT);
}


//Main menu
void menu_main_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	for(i=0; i<8&&widget!=menu[MENU_STATE_MAIN].button[i]; i++);
	if(i==0) {
		ui_messagebox(font_std, T("Singleplayer is not yet availble."));
		return;
	}
	
	menu_state(MENU_STATE_SINGLEPLAYER+i);
}

void menu_singleplayer_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	//TODO: implement singleplayer, etc... olol
}

void menu_multiplayer_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget==menu[MENU_STATE_MULTIPLAYER].button[0]) {
		panelist_menu_sidebar.next=&panelist_multiplayer_host;
		ui_selected_widget=multiplayer_host_entry_port;
	} else if(widget==menu[MENU_STATE_MULTIPLAYER].button[1]) {
		panelist_menu_sidebar.next=&panelist_multiplayer_join;
		ui_selected_widget=multiplayer_join_entry_host;
	}
}

void menu_settings_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget==menu[MENU_STATE_SETTINGS].button[0]) {
		panelist_menu_sidebar.next=&panelist_settings_game;
		ui_selected_widget=settings_game_entry_name;
	} else if(widget==menu[MENU_STATE_SETTINGS].button[1]) {
		panelist_menu_sidebar.next=&panelist_settings_monitor;
		ui_selected_widget=NULL;
	}
		
}

void menu_sidebar_button_quit_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(menustate==MENU_STATE_MAIN)
		game_state(GAME_STATE_QUIT);
	else {
		menu_state(MENU_STATE_MAIN);
		panelist_menu_sidebar.next=NULL;
		ui_selected_widget=NULL;
	}
}

void menu_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(e->buttons->start&&!prevbuttons.start) {
		switch(gamestate) {
			case GAME_STATE_MENU:
				game_state(GAME_STATE_QUIT);
				break;
			case GAME_STATE_GAME_MENU:
				game_state(GAME_STATE_GAME);
				break;
			default:
				game_state(GAME_STATE_MENU);
				break;
		}
	}
	
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
