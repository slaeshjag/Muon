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

#ifndef CHECKBOX_H
#define CHECKBOX_H

#define UI_CHECKBOX_PROP_ACTIVATED 0
#define UI_CHECKBOX_PROP_BORDER 1
#define UI_CHECKBOX_PROP_ACTIVE_BORDER 2

struct UI_CHECKBOX_PROPERTIES {
	int activated;
	DARNIT_LINE *border;
	DARNIT_LINE *active_border;
};

UI_WIDGET *ui_widget_create_checkbox();
void *ui_widget_destroy_checkbox(UI_WIDGET *widget);

void ui_checkbox_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void ui_checkbox_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_checkbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_checkbox_get_prop(UI_WIDGET *widget, int prop);
void ui_checkbox_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_checkbox_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_checkbox_render(UI_WIDGET *widget);

#endif
