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

#ifndef SPACER_H
#define SPACER_H

struct UI_SPACER_PROPERTIES {
	int set_w;
	int set_h;
};

UI_WIDGET *ui_widget_create_spacer();
UI_WIDGET *ui_widget_create_spacer_size(int w, int h);

UI_PROPERTY_VALUE ui_spacer_get_prop(UI_WIDGET *widget, int prop);
void ui_spacer_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
void ui_spacer_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_spacer_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_spacer_render(UI_WIDGET *widget);

#endif
