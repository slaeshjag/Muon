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

#include "ui.h"

UI_PANE *ui_pane_create(int x, int y, int w, int h, UI_WIDGET *root_widget) {
	UI_PANE *pane;
	if((pane=malloc(sizeof(UI_PANE)))==NULL)
		return NULL;
	pane->x=x; pane->y=y;
	pane->w=w; pane->h=h;
	pane->background_color.r=pane->background_color.g=pane->background_color.b=0;
	pane->background_color.a=255;
	pane->background=d_render_rect_new(1);
	pane->border=d_render_line_new(4, 2);
	pane->root_widget=NULL;
	ui_pane_resize(pane, x, y, w ,h);
	ui_pane_set_root_widget(pane, root_widget);
	
	return pane;
}

void *ui_pane_destroy(UI_PANE *pane) {
	d_render_line_free(pane->border);
	d_render_rect_free(pane->background);
	free(pane);
	return NULL;
}

void ui_pane_resize(UI_PANE *pane, int x, int y, int w, int h) {
	d_render_rect_move(pane->background, 0, x, y, x+w, y+h);
	d_render_line_move(pane->border, 0, x, y, x+w, y);
	d_render_line_move(pane->border, 1, x, y+h, x+w, y+h);
	d_render_line_move(pane->border, 2, x, y, x, y+h);
	d_render_line_move(pane->border, 3, x+w, y, x+w, y+h);
	if(pane->root_widget!=NULL)
		pane->root_widget->resize(pane->root_widget, x+UI_PADDING, y+UI_PADDING, w-UI_PADDING*2, h-UI_PADDING*2);
}

void ui_pane_set_root_widget(UI_PANE *pane, UI_WIDGET *root_widget) {
	pane->root_widget=root_widget;
	if(pane->root_widget!=NULL)
		pane->root_widget->resize(pane->root_widget, pane->x+UI_PADDING, pane->y+UI_PADDING, pane->w-UI_PADDING*2, pane->h-UI_PADDING*2);
}

void ui_pane_render(UI_PANE *pane) {
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(pane->background_color.r, pane->background_color.g, pane->background_color.b, pane->background_color.a);
	d_render_rect_draw(pane->background, 1);
	d_render_tint(r, g, b, a);
	d_render_line_draw(pane->border, 4);
	
	if(pane->root_widget!=NULL)
		pane->root_widget->render(pane->root_widget);
}

void ui_panelist_render(struct UI_PANE_LIST *panelist) {
	for(; panelist; panelist=panelist->next)
		ui_pane_render(panelist->pane);
}
