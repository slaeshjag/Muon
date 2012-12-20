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

#ifndef UI_H
#define UI_H

#include <darnit/darnit.h>

#define UI_PADDING 4

//#include "../muon.h"

typedef union {
	void *p;
	int i;
} UI_PROPERTY_VALUE;

typedef struct UI_WIDGET {
	void *(*destroy)(struct UI_WIDGET *);
	void (*set_prop)(struct UI_WIDGET *, int, UI_PROPERTY_VALUE);
	UI_PROPERTY_VALUE (*get_prop)(struct UI_WIDGET *, int);
	/*void (*event)(struct UI_WIDGET *, int, int);*/
	void (*resize)(struct UI_WIDGET *, int, int, int, int);
	void (*request_size)(struct UI_WIDGET *, int *, int *);
	void (*render)(struct UI_WIDGET *);
	void *properties;
	struct UI_EVENT_HANDLER *event_handler;
	int x;
	int y;
	int w;
	int h;
	unsigned int enabled:1;
} UI_WIDGET;

UI_WIDGET *ui_selected_widget;

#include "event.h"

#include "pane.h"
#include "label.h"
#include "vbox.h"
#include "hbox.h"
#include "button.h"
#include "checkbox.h"
#include "entry.h"
#include "progressbar.h"
#include "listbox.h"
#include "spacer.h"
#include "imageview.h"
#include "slider.h"

void ui_init();
void *ui_widget_destroy(UI_WIDGET *widget);

#endif
