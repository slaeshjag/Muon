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
	widget->event_handler->send=ui_event_send;
	widget->event_handler->add(widget, ui_checkbox_event_click, UI_EVENT_TYPE_MOUSE);
	widget->event_handler->add(widget, ui_checkbox_event_key, UI_EVENT_TYPE_KEYBOARD);
	
	struct UI_CHECKBOX_PROPERTIES *p=widget->properties;
	p->activated=0;
	p->border=darnitRenderLineAlloc(4, 1);
	p->active_border=darnitRenderLineAlloc(2, 1);
	widget->set_prop=ui_checkbox_set_prop;
	widget->get_prop=ui_checkbox_get_prop;
	widget->resize=ui_checkbox_resize;
	widget->request_size=ui_checkbox_request_size;
	widget->render=ui_checkbox_render;
	
	widget->x=widget->y=widget->w=widget->h=0;

	return widget;
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
		case UI_EVENT_TYPE_MOUSE_DOWN:
			if(e->mouse->buttons==UI_EVENT_MOUSE_BUTTON_LEFT)
				ui_selected_widget=widget;
				p->activated=!p->activated;
				UI_EVENT ee;
				UI_EVENT_UI e_u={};
				ee.ui=&e_u;
				widget->event_handler->send(widget, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE, &ee);
			break;
		/*case UI_EVENT_TYPE_MOUSE_UP:
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