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

#ifndef LABEL_H
#define LABEL_H

#define UI_LABEL_PROP_FONT 0
#define UI_LABEL_PROP_SURFACE 1
#define UI_LABEL_PROP_TEXT 2

struct UI_LABEL_PROPERTIES {
	DARNIT_FONT *font;
	DARNIT_TEXT_SURFACE *surface;
	char *text;
};

UI_WIDGET *ui_widget_create_label(DARNIT_FONT *font, const char *text);
void *ui_widget_destroy_label(UI_WIDGET *widget);

void ui_label_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_label_get_prop(UI_WIDGET *widget, int prop);
void ui_label_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_label_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_label_render(UI_WIDGET *widget);

#endif
