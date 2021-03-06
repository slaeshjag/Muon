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

#include "muon.h"
#include "view.h"
#include "client.h"
#include "game.h"
#include "menu/menu.h"
#include "gameover.h"

const char *gameover_stats_headers[6]={
	"Built",
	"Lost",
	"Destroyed",
	"Efficiency",
	"Score",
	"Player",
};

void gameover_init() {
	int i;
	panelist_gameover_sidebar.pane=ui_pane_create(platform.screen_w-sidebar_width, 0, sidebar_width, platform.screen_h, ui_widget_create_vbox());
	panelist_gameover_sidebar.next=&panelist_gameover_stats;
	panelist_gameover_stats.pane=ui_pane_create((platform.screen_w-sidebar_width)/2-480/2, platform.screen_h/2-240/2, 480, 240, ui_widget_create_vbox());
	panelist_gameover_stats.next=NULL;
	
	gameover_sidebar_button_menu=ui_widget_create_button_text(font_std, T("Menu"));
	gameover_sidebar_button_menu->event_handler->add(gameover_sidebar_button_menu, gameover_sidebar_button_menu_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_vbox_add_child(panelist_gameover_sidebar.pane->root_widget, ui_widget_create_label(font_std, T("Game over")), 0);
	ui_vbox_add_child(panelist_gameover_sidebar.pane->root_widget, ui_widget_create_spacer(), 1);
	ui_vbox_add_child(panelist_gameover_sidebar.pane->root_widget, gameover_sidebar_button_menu, 0);
	
	gameover_stats_label_time=ui_widget_create_label(font_std, T("Game time"));
	gameover_stats_hbox=ui_widget_create_hbox();
	
	for(i=0; i<5; i++) {
		gameover_stats_vbox_stat[i]=ui_widget_create_vbox();
		ui_vbox_add_child(gameover_stats_vbox_stat[i], ui_widget_create_label(font_std, T(gameover_stats_headers[i])), 0);
		ui_hbox_add_child(gameover_stats_hbox, gameover_stats_vbox_stat[i], 0);
		ui_hbox_add_child(gameover_stats_hbox, ui_widget_create_spacer(), 1);
	}
	gameover_stats_vbox_stat[i]=ui_widget_create_vbox();
	ui_vbox_add_child(gameover_stats_vbox_stat[i], ui_widget_create_label(font_std, T(gameover_stats_headers[i])), 0);
	ui_hbox_add_child(gameover_stats_hbox, gameover_stats_vbox_stat[i], 0);
	//ui_hbox_add_child(gameover_stats_hbox, ui_widget_create_spacer_size(1, 1), 0);
	
	ui_vbox_add_child(panelist_gameover_stats.pane->root_widget, ui_widget_create_label(font_std, T("Game statistics")), 0);
	ui_vbox_add_child(panelist_gameover_stats.pane->root_widget, gameover_stats_label_time, 0);
	ui_vbox_add_child(panelist_gameover_stats.pane->root_widget, gameover_stats_hbox, 0);
	
	gameover_statlabel=NULL;
}

void gameover_update_stats() {
	//don't look at this function! it is probably the ugliest thing i've ever made!
	int i;
	UI_PROPERTY_VALUE v;
	unsigned int h, m, s, t=(d_time_get()-game_time_start)/1000;
	
	if(gameover_statlabel) {
		for(i=0; i<players; i++) {
			if(!(gameover_statlabel[i].built&&gameover_statlabel[i].lost&&gameover_statlabel[i].destroyed&&gameover_statlabel[i].efficiency&&gameover_statlabel[i].score&&gameover_statlabel[i].name))
				continue;
			ui_vbox_remove_child(gameover_stats_vbox_stat[0], gameover_statlabel[i].built);
			ui_vbox_remove_child(gameover_stats_vbox_stat[1], gameover_statlabel[i].lost);
			ui_vbox_remove_child(gameover_stats_vbox_stat[2], gameover_statlabel[i].destroyed);
			ui_vbox_remove_child(gameover_stats_vbox_stat[3], gameover_statlabel[i].efficiency);
			ui_vbox_remove_child(gameover_stats_vbox_stat[4], gameover_statlabel[i].score);
			ui_vbox_remove_child(gameover_stats_vbox_stat[5], gameover_statlabel[i].name);
			gameover_statlabel[i].built->destroy(gameover_statlabel[i].built);
			gameover_statlabel[i].lost->destroy(gameover_statlabel[i].lost);
			gameover_statlabel[i].destroyed->destroy(gameover_statlabel[i].destroyed);
			gameover_statlabel[i].efficiency->destroy(gameover_statlabel[i].efficiency);
			gameover_statlabel[i].score->destroy(gameover_statlabel[i].score);
			gameover_statlabel[i].name->destroy(gameover_statlabel[i].name);
		}
		free(gameover_statlabel);
	}
	
	s=t%60;
	m=t/60;
	h=m/60;
	m=m%60;
	sprintf(gameover_stats_label_time_text, "%s: %02u:%02u:%02u", T("Game time"), h, m, s);
	v.p=gameover_stats_label_time_text;
	gameover_stats_label_time->set_prop(gameover_stats_label_time, UI_LABEL_PROP_TEXT, v);
	
	if((gameover_statlabel=malloc(sizeof(struct GAMEOVER_STATLABEL)*players))==NULL)
		return;
	for(i=0; i<players; i++) {
		if(!player[i].name[0]) {
			gameover_statlabel[i].built=gameover_statlabel[i].lost=gameover_statlabel[i].destroyed=gameover_statlabel[i].efficiency=gameover_statlabel[i].score=gameover_statlabel[i].name=NULL;
			continue;
		}
		sprintf(gameover_statlabel[i].built_text, "%i", player[i].stats.built);
		sprintf(gameover_statlabel[i].lost_text, "%i", player[i].stats.lost);
		sprintf(gameover_statlabel[i].destroyed_text, "%i", player[i].stats.destroyed);
		sprintf(gameover_statlabel[i].efficiency_text, "%i%%", player[i].stats.efficiency);
		sprintf(gameover_statlabel[i].score_text, "%i", player[i].stats.score);
		ui_vbox_add_child(gameover_stats_vbox_stat[0], gameover_statlabel[i].built=ui_widget_create_label(font_std, gameover_statlabel[i].built_text), 0);
		ui_vbox_add_child(gameover_stats_vbox_stat[1], gameover_statlabel[i].lost=ui_widget_create_label(font_std, gameover_statlabel[i].lost_text), 0);
		ui_vbox_add_child(gameover_stats_vbox_stat[2], gameover_statlabel[i].destroyed=ui_widget_create_label(font_std, gameover_statlabel[i].destroyed_text), 0);
		ui_vbox_add_child(gameover_stats_vbox_stat[3], gameover_statlabel[i].efficiency=ui_widget_create_label(font_std, gameover_statlabel[i].efficiency_text), 0);
		ui_vbox_add_child(gameover_stats_vbox_stat[4], gameover_statlabel[i].score=ui_widget_create_label(font_std, gameover_statlabel[i].score_text), 0);
		ui_vbox_add_child(gameover_stats_vbox_stat[5], gameover_statlabel[i].name=ui_widget_create_label(font_std, player[i].name), 0);
	}
	
}

void gameover_sidebar_button_menu_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	game_state(GAME_STATE_MENU);
	menu_state(MENU_STATE_MAIN);
}
