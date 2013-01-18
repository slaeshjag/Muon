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

#ifndef PANE_H
#define PANE_H

typedef struct {
	int x;
	int y;
	int w;
	int h;
	DARNIT_LINE *border;
	DARNIT_RECT *background;
	UI_WIDGET *root_widget;
} UI_PANE;

struct UI_PANE_LIST {
	UI_PANE *pane;
	struct UI_PANE_LIST *next;
};

UI_PANE *ui_pane_create(int x, int y, int w, int h, UI_WIDGET *root_widget);
void *ui_pane_destroy(UI_PANE *pane);

void ui_pane_resize(UI_PANE *pane, int x, int y, int w, int h);
void ui_pane_set_root_widget(UI_PANE *pane, UI_WIDGET *root_widget);
void ui_pane_render(UI_PANE *pane);
void ui_panelist_render(struct UI_PANE_LIST *panelist);

#endif
