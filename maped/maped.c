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
#include "editor.h"

void state_set(enum STATE state) {
	switch(state_current) {
		case STATE_EDITOR:
			ui_event_global_remove(editor_mouse_draw, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_remove(editor_mouse_click, UI_EVENT_TYPE_MOUSE_PRESS);
		default:
			break;
	}
	switch(state) {
		case STATE_EDITOR:
			ui_event_global_add(editor_mouse_click, UI_EVENT_TYPE_MOUSE_PRESS);
			ui_event_global_add(editor_mouse_draw, UI_EVENT_TYPE_MOUSE_ENTER);
		default:
			break;
	}
	state_current=state;
}

int main(int argc, char **argv) {
	platform_init("Muon map editor", "res/icon-maped.png");
	font_std=d_font_load("res/FreeMonoBold.ttf", 12, 512, 512);
	mouse_tilesheet=d_render_tilesheet_load("res/mouse.png", 16, 16, DARNIT_PFORMAT_RGB5A1);
	
	menu_init();
	editor_init();
	
	ui_event_global_add(view_mouse_draw, UI_EVENT_TYPE_UI);
	d_render_clearcolor_set(0x50, 0x50, 0x50);
	
	state_current=STATE_MENU;
	
	while(state_current!=STATE_QUIT) {
		d_render_begin();
		d_render_tint(255, 255, 255, 255);
		if(state[state_current].render)
			state[state_current].render();
		d_render_tint(255, 0, 0, 255);
		ui_events(state[state_current].panelist, 1);
		d_render_end();
		d_loop();
	}
	
	d_quit();
	return 0;
}

void view_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(255, 255, 255, 255);
	d_render_blend_enable();
	d_render_tile_blit(mouse_tilesheet, 0, e->mouse->x, e->mouse->y);
	d_render_blend_disable();
	d_render_tint(r, g, b, a);
}
