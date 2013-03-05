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

#include "../muon.h"
#include "../intmath.h"
#include "../view.h"
#include "../platform.h"
#include "credits.h"

char credits_text[256];
char credits_contact_text[128];

void credits_init() {
	sprintf(credits_text, "%s:\n    %s\n    %s\n\n%s:\n    %s\n    %s\n\n%s:\n    %s\n    %s",
		T("Server-side programmer"), "slaeshjag", "http://rdw.se/",
		T("Client-side programmer"), "h4xxel", "http://h4xxel.org/",
		T("Maps and graphics"), "~kqr", "http://hasp.xkqr.org/"
	);
	panelist_credits.pane=ui_pane_create(platform.screen_w/2-320/2, platform.screen_h/2-240/2, 320, 240, ui_widget_create_vbox());
	panelist_credits.next=&panelist_credits_contact;
	ui_vbox_add_child(panelist_credits.pane->root_widget, ui_widget_create_label(font_std, T("Credits")), 0);
	ui_vbox_add_child(panelist_credits.pane->root_widget, ui_widget_create_label(font_std, credits_text), 0);
	ui_vbox_add_child(panelist_credits.pane->root_widget, ui_widget_create_spacer(), 1);
	ui_vbox_add_child(panelist_credits.pane->root_widget, credits_button_back=ui_widget_create_button_text(font_std, T("Back")), 0);
	credits_button_back->event_handler->add(credits_button_back, credits_button_back_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	
	sprintf(credits_contact_text, "%s: %s\n%s: %s",
		T("Bug tracker"), "http://github.com/slaeshjag/Muon/",
		T("Contact"), "muon@rdw.se"
	);
	panelist_credits_contact.pane=ui_pane_create(0, 0, 256, 48, ui_widget_create_label(font_std, credits_contact_text));
	
	if(platform.platform&DARNIT_PLATFORM_PANDORA) {
		panelist_credits_dbcc.pane=ui_pane_create(config.screen_w-(200+ui_padding*2), 0, 200+ui_padding*2, 187+ui_padding*2, ui_widget_create_imageview_file("res/dragonbox_compo.png", 200, 187, DARNIT_PFORMAT_RGB5A1));
		panelist_credits_contact.next=&panelist_credits_dbcc;
		panelist_credits_dbcc.next=NULL;
	}
}

void credits_button_back_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	game_state(GAME_STATE_MENU);
}

void credits_background_update(int t) {
	register int x, y, mov1, mov2, c1, c2, c3, c;
	for (y=0; y<view_background_h; y++)
		for (x=0; x<view_background_w; x++) {
			mov1=(y<<8)/view_background_h+(t>>1);
			mov2=(x<<8)/view_background_w;
			c1=sine(mov1+(t>>1))/2+((mov2>>1)-mov1-mov2+(t>>1));
			c2=sine((c1+sine((y>>2)+(t>>1))+sine((/*x+*/100+y)))/8);
			c3=sine((c2+(cosine(mov1+mov2+c2/8)>>2)+cosine(mov2)+sine(x))/8);
			c=(c1+c2+c3)/256+32;
			view_background_pixbuf[y*view_background_w+x]=c|c<<8|c<<16;
		}
	d_render_tilesheet_update(view_background_ts, 0, 0, view_background_w, view_background_h, view_background_pixbuf);
}

void credits_background_draw() {
	static int t=0, recalc=1;
	if(config.plasma&&recalc) {
		credits_background_update(t);
		t++;
	}
	recalc=!recalc;
	
	d_render_tile_draw(view_background_tile, 1);
}

void credits_draw() {
	static int fade=0, tint=-127;
	//printf("arne %i\n", d_render_fade_status());
	if(d_render_fade_status()==1) {
		view_background_draw();
		fade=1;
		gamestate_pane[GAME_STATE_CREDITS]=NULL;
		return;
	}
	gamestate_pane[GAME_STATE_CREDITS]=&panelist_credits;
	if(fade) {
		d_render_fade_out(500);
		fade=0;
	}
	d_render_tint(0, 0, 127+ABS(tint)%128, 255);
	credits_background_draw();
	d_render_tint(255, 255, 255, 255);
	if(++tint==128)
		tint=-127;
}
