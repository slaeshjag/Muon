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

#include <stdio.h>

#include "ui.h"

UI_WIDGET *ui_widget_create_slider(unsigned int steps) {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_SLIDER_PROPERTIES)))==NULL) {
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
	widget->event_handler->add(widget, ui_slider_event_mouse_down, UI_EVENT_TYPE_MOUSE_DOWN);
	widget->event_handler->add(widget, ui_slider_event_mouse_release, UI_EVENT_TYPE_MOUSE_RELEASE);
	widget->event_handler->add(widget, ui_slider_event_mouse_release, UI_EVENT_TYPE_MOUSE_LEAVE);
	
	struct UI_SLIDER_PROPERTIES *p=widget->properties;
	p->line=darnitRenderLineAlloc(1+steps, 1);
	p->handle=darnitRenderRectAlloc(1);
	p->value=0;
	p->steps=steps;
	
	widget->destroy=ui_widget_destroy_slider;
	widget->set_prop=ui_slider_set_prop;
	widget->get_prop=ui_slider_get_prop;
	widget->resize=ui_slider_resize;
	widget->request_size=ui_slider_request_size;
	widget->render=ui_slider_render;
	widget->x=widget->y=widget->w=widget->h=0;
	widget->enabled=1;

	return widget;
}

void *ui_widget_destroy_slider(UI_WIDGET *widget) {
	struct UI_SLIDER_PROPERTIES *p=widget->properties;
	darnitRenderRectFree(p->handle);
	darnitRenderLineFree(p->line);
	return ui_widget_destroy(widget);
}

void ui_slider_event_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(!widget->enabled)
		return;
	int i, value=0;
	struct UI_SLIDER_PROPERTIES *p=widget->properties;
	for(i=0; i<p->steps; i++)
		if(e->mouse->x>widget->x+2+(widget->w-4)/(p->steps-1)/2+i*((widget->w-4)/(p->steps-1)))
			value++;
	p->value=value;
	widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
}

void ui_slider_event_mouse_release(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type==UI_EVENT_TYPE_MOUSE_LEAVE&&!e->mouse->buttons)
		return;
	UI_EVENT ee;
	UI_EVENT_UI e_u={};
	ee.ui=&e_u;
	widget->event_handler->send(widget, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE, &ee);
}

void ui_slider_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_SLIDER_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_SLIDER_PROP_VALUE:
			p->value=value.i;
			if(p->value>=p->steps)
				p->value=p->steps-1;
			widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
			break;
		case UI_SLIDER_PROP_STEPS:
			p->steps=value.i;
			if(p->value>=p->steps)
				p->value=p->steps-1;
			widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
			break;
	}
}

UI_PROPERTY_VALUE ui_slider_get_prop(UI_WIDGET *widget, int prop) {
	UI_PROPERTY_VALUE v={.p=NULL};
	struct UI_SLIDER_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_SLIDER_PROP_VALUE:
			v.i=p->value;
			break;
		case UI_SLIDER_PROP_STEPS:
			v.i=p->steps;
			break;
	}
	return v;
}

void ui_slider_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	if(h==0||h==0)
		return;
	struct UI_SLIDER_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	
	darnitRenderLineMove(p->line, 0, x+2, y+h/2, x+w-2, y+h/2);
	int i;
	for(i=0; i<p->steps; i++) {
		int xx=x+2+i*((w-4)/(p->steps-1));
		darnitRenderLineMove(p->line, i+1, xx, y+h/2-2, xx, y+h/2+2);
	}
	int xx=x+2+p->value*((w-4)/(p->steps-1));
	darnitRenderRectSet(p->handle, 0, xx-2, y+h/2-4, xx+2, y+h/2+4);
}

void ui_slider_request_size(UI_WIDGET *widget, int *w, int *h) {
	if(w)
		*w=64;
	if(h)
		*h=8;
}

void ui_slider_render(UI_WIDGET *widget) {
	struct UI_SLIDER_PROPERTIES *p=widget->properties;
	darnitRenderLineDraw(p->line, 1+p->steps);
	darnitRenderRectDraw(p->handle, 1);
}
