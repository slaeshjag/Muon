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

#ifndef BUTTON_H
#define BUTTON_H

#define UI_BUTTON_PROP_CHILD 0
#define UI_BUTTON_PROP_ACTIVATED 1
#define UI_BUTTON_PROP_BORDER 2
#define UI_BUTTON_PROP_ACTIVE_BORDER 3

struct UI_BUTTON_PROPERTIES {
	UI_WIDGET *child;
	int activated;
	DARNIT_LINE *border;
	DARNIT_LINE *active_border;
};

UI_WIDGET *ui_widget_create_button(UI_WIDGET *child);
UI_WIDGET *ui_widget_create_button_text(DARNIT_FONT *font, const char *text);
UI_WIDGET *ui_widget_create_button_image();

void *ui_widget_destroy_button(UI_WIDGET *widget);
void *ui_widget_destroy_button_recursive(UI_WIDGET *widget);

void ui_button_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void ui_button_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_button_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_button_get_prop(UI_WIDGET *widget, int prop);
void ui_button_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_button_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_button_render(UI_WIDGET *widget);

#endif
