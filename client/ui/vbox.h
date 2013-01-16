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

#ifndef VBOX_H
#define VBOX_H

#include "box.h"

#define UI_VBOX_PROP_CHILDREN 0
#define UI_VBOX_PROP_SIZE 1

struct UI_VBOX_PROPERTIES {
	UI_WIDGET_LIST *children;
	int size;
};

UI_WIDGET *ui_widget_create_vbox();
void *ui_widget_vbox_destroy(UI_WIDGET* widget);

void ui_vbox_event_notify_children(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_vbox_add_child(UI_WIDGET *widget, UI_WIDGET *child, int expand);
void ui_vbox_remove_child(UI_WIDGET *widget, UI_WIDGET *child);
void ui_vbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_vbox_get_prop(UI_WIDGET *widget, int prop);
void ui_vbox_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_vbox_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_vbox_render(UI_WIDGET *widget);

#endif
