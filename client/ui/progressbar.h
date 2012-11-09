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

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#define UI_PROGRESSBAR_PROP_FONT 0
#define UI_PROGRESSBAR_PROP_SURFACE 1
#define UI_PROGRESSBAR_PROP_BORDER 2
#define UI_PROGRESSBAR_PROP_BAR 3
#define UI_PROGRESSBAR_PROP_PROGRESS 4
#define UI_PROGRESSBAR_PROP_TEXT 5

struct UI_PROGRESSBAR_PROPERTIES {
	DARNIT_FONT *font;
	DARNIT_TEXT_SURFACE *surface;
	DARNIT_LINE *border;
	DARNIT_RECT *bar;
	int progress;
	char text[5];
};

UI_WIDGET *ui_widget_create_progressbar(DARNIT_FONT *font);
void *ui_widget_destroy_progressbar(UI_WIDGET *widget);

void ui_progressbar_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_progressbar_get_prop(UI_WIDGET *widget, int prop);
void ui_progressbar_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_progressbar_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_progressbar_render(UI_WIDGET *widget);

#endif
