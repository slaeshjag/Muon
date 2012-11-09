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
#include "../view.h"

UI_WIDGET *ui_widget_create_checkbox() {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_CHECKBOX_PROPERTIES)))==NULL) {
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
	widget->event_handler->add(widget, ui_checkbox_event_click, UI_EVENT_TYPE_MOUSE);
	widget->event_handler->add(widget, ui_checkbox_event_key, UI_EVENT_TYPE_KEYBOARD);
	
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	p->activated=0;
	p->border=darnitRenderLineAlloc(4, 1);
	p->active_border=darnitRenderLineAlloc(2, 1);
	widget->destroy=ui_widget_destroy_checkbox;
	widget->set_prop=ui_checkbox_set_prop;
	widget->get_prop=ui_checkbox_get_prop;
	widget->resize=ui_checkbox_resize;
	widget->request_size=ui_checkbox_request_size;
	widget->render=ui_checkbox_render;
	
	widget->x=widget->y=widget->w=widget->h=0;

	return widget;
}

void *ui_widget_destroy_checkbox(UI_WIDGET *widget) {
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	darnitRenderLineFree(p->border);
	darnitRenderLineFree(p->active_border);
	return ui_widget_destroy(widget);
}

void ui_checkbox_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	if(e->keyboard->keysym!=KEY(RETURN)&&e->keyboard->keysym!=KEY(KP_ENTER)&&e->keyboard->keysym!=KEY(SPACE))
		return;
	switch(type) {
		case UI_EVENT_TYPE_KEYBOARD_PRESS:
			p->activated=!p->activated;
			break;
		case UI_EVENT_TYPE_KEYBOARD_RELEASE:
			break;
	}
}

void ui_checkbox_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	switch(type) {
		case UI_EVENT_TYPE_MOUSE_PRESS:
			if(e->mouse->buttons==UI_EVENT_MOUSE_BUTTON_LEFT)
				ui_selected_widget=widget;
				p->activated=!p->activated;
				UI_EVENT ee;
				UI_EVENT_UI e_u={};
				ee.ui=&e_u;
				widget->event_handler->send(widget, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE, &ee);
			break;
		/*case UI_EVENT_TYPE_MOUSE_RELEASE:
			if(((e->mouse->buttons)&UI_EVENT_MOUSE_BUTTON_LEFT)==UI_EVENT_MOUSE_BUTTON_LEFT)
				break;
		case UI_EVENT_TYPE_MOUSE_LEAVE:
			p->activated=0;
			break;*/
	}
}

void ui_checkbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_CHECKBOX_PROP_ACTIVATED:
			p->activated=value.i;
			break;
		case UI_CHECKBOX_PROP_BORDER:
			break;
		case UI_CHECKBOX_PROP_ACTIVE_BORDER:
			break;
	}
}

UI_PROPERTY_VALUE ui_checkbox_get_prop(UI_WIDGET *widget, int prop) {
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	UI_PROPERTY_VALUE v={.p=NULL};
	switch(prop) {
		case UI_CHECKBOX_PROP_ACTIVATED:
			v.i=p->activated;
			break;
		case UI_CHECKBOX_PROP_BORDER:
			v.p=p->border;
			break;
		case UI_CHECKBOX_PROP_ACTIVE_BORDER:
			v.p=p->active_border;
			break;
	}
	return v;
}

void ui_checkbox_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	
	darnitRenderLineMove(p->border, 0, x+w/2-4, y+h/2-4, x+w/2+4, y+h/2-4);
	darnitRenderLineMove(p->border, 1, x+w/2-4, y+h/2+4, x+w/2+4, y+h/2+4);
	darnitRenderLineMove(p->border, 2, x+w/2-4, y+h/2-4, x+w/2-4, y+h/2+4);
	darnitRenderLineMove(p->border, 3, x+w/2+4, y+h/2-4, x+w/2+4, y+h/2+4);

	
	darnitRenderLineMove(p->active_border, 0, x+w/2-4, y+h/2-4, x+w/2+4, y+h/2+4);
	darnitRenderLineMove(p->active_border, 1, x+w/2+4, y+h/2-4, x+w/2-4, y+h/2+4);
}

void ui_checkbox_request_size(UI_WIDGET *widget, int *w, int *h) {
	if(w)
		(*w)=8;
	if(h)
		(*h)=8;
}

void ui_checkbox_render(UI_WIDGET *widget) {
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	darnitRenderLineDraw(p->border, 4);
	if(p->activated)
		darnitRenderLineDraw(p->active_border, 2);
}
