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

UI_WIDGET *ui_widget_create_spacer() {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_SPACER_PROPERTIES)))==NULL) {
		free(widget);
		return NULL;
	}
	
	if((widget->event_handler=malloc(sizeof(UI_EVENT_HANDLER)))==NULL) {
		free(widget->properties);
		free(widget);
		return NULL;
	}
	widget->event_handler->handlers=NULL;
	widget->event_handler->add=ui_event_add;
	widget->event_handler->remove=ui_event_remove;
	widget->event_handler->send=ui_event_send;
	
	struct UI_SPACER_PROPERTIES *p=widget->properties;
	p->set_w=0;
	p->set_h=0;
	
	widget->destroy=ui_widget_destroy;
	widget->set_prop=ui_spacer_set_prop;
	widget->get_prop=ui_spacer_get_prop;
	widget->resize=ui_spacer_resize;
	widget->request_size=ui_spacer_request_size;
	widget->render=ui_spacer_render;
	widget->x=widget->y=widget->w=widget->h=0;
	widget->enabled=1;
	
	return widget;
}

UI_WIDGET *ui_widget_create_spacer_size(int w, int h) {
	UI_WIDGET *widget;
	widget=ui_widget_create_spacer();
	struct UI_SPACER_PROPERTIES *p=widget->properties;
	p->set_w=w;
	p->set_h=h;
	return widget;
}

UI_PROPERTY_VALUE ui_spacer_get_prop(UI_WIDGET *widget, int prop) {
	UI_PROPERTY_VALUE v={.p=NULL};
	return v;
}

void ui_spacer_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	return;
}

void ui_spacer_request_size(UI_WIDGET *widget, int *w, int *h) {
	struct UI_SPACER_PROPERTIES *p=widget->properties;
	if(w)
		*w=p->set_w;
	if(h)
		*h=p->set_h;
}

void ui_spacer_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
}

void ui_spacer_render(UI_WIDGET *widget) {
	return;
}
