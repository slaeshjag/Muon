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

#ifndef SLIDER_H
#define SLIDER_H

#define UI_SLIDER_PROP_LINE 1
#define UI_SLIDER_PROP_HANDLE 2
#define UI_SLIDER_PROP_VALUE 3
#define UI_SLIDER_PROP_STEPS 4

struct UI_SLIDER_PROPERTIES {
	DARNIT_LINE *line;
	DARNIT_RECT *handle;
	unsigned int value;
	unsigned int steps;
};

UI_WIDGET *ui_widget_create_slider(unsigned int steps);
void *ui_widget_destroy_slider(UI_WIDGET *widget);

void ui_slider_event_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void ui_slider_event_mouse_release(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_slider_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_slider_get_prop(UI_WIDGET *widget, int prop);
void ui_slider_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_slider_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_slider_render(UI_WIDGET *widget);

#endif
