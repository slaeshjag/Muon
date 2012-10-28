#include "ui.h"

UI_WIDGET *ui_widget_create_button(UI_WIDGET *child) {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_BUTTON_PROPERTIES)))==NULL) {
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
	widget->event_handler->add(widget, ui_button_event_click, UI_EVENT_TYPE_MOUSE);
	widget->event_handler->add(widget, ui_button_event_key, UI_EVENT_TYPE_KEYBOARD);
	
	struct UI_BUTTON_PROPERTIES *p=widget->properties;
	p->child=child;
	p->activated=0;
	p->border=darnitRenderLineAlloc(8, 1);
	p->active_border=darnitRenderLineAlloc(4, 1);
	widget->set_prop=ui_button_set_prop;
	widget->get_prop=ui_button_get_prop;
	widget->resize=ui_button_resize;
	widget->request_size=ui_button_request_size;
	widget->render=ui_button_render;
	
	widget->x=widget->y=widget->w=widget->h=0;

	return widget;
}

UI_WIDGET *ui_widget_create_button_text(char *text) {
	UI_WIDGET *widget, *label;
	label=ui_widget_create_label(font_std, text);
	widget=ui_widget_create_button(label);
	widget->destroy=ui_widget_destroy_button_text;
	return widget;
}

UI_WIDGET *ui_widget_create_button_image() {
	return NULL;
}

void ui_widget_destroy_button_text(UI_WIDGET *widget) {
	return;
}

void ui_button_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	struct UI_BUTTON_PROPERTIES *p=widget->properties;
	if(e->keyboard->keysym!=KEY(RETURN)&&e->keyboard->keysym!=KEY(KP_ENTER)&&e->keyboard->keysym!=KEY(SPACE))
		return;
	switch(type) {
		case UI_EVENT_TYPE_KEYBOARD_PRESS:
			p->activated=1;
			break;
		case UI_EVENT_TYPE_KEYBOARD_RELEASE:
			p->activated=0;
			break;
	}
}

void ui_button_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	struct UI_BUTTON_PROPERTIES *p=widget->properties;
	switch(type) {
		case UI_EVENT_TYPE_MOUSE_DOWN:
			if(e->mouse->buttons==UI_EVENT_MOUSE_BUTTON_LEFT)
				ui_selected_widget=widget;
				p->activated=1;
				UI_EVENT ee;
				UI_EVENT_UI e_u={};
				ee.ui=&e_u;
				widget->event_handler->send(widget, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE, &ee);
			break;
		case UI_EVENT_TYPE_MOUSE_UP:
			if(((e->mouse->buttons)&UI_EVENT_MOUSE_BUTTON_LEFT)==UI_EVENT_MOUSE_BUTTON_LEFT)
				break;
		case UI_EVENT_TYPE_MOUSE_LEAVE:
			p->activated=0;
			break;
	}
}

void ui_button_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_BUTTON_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_BUTTON_PROP_CHILD:
			break;
		case UI_BUTTON_PROP_ACTIVATED:
			p->activated=value.i;
			break;
		case UI_BUTTON_PROP_BORDER:
			break;
		case UI_BUTTON_PROP_ACTIVE_BORDER:
			break;
	}
}

UI_PROPERTY_VALUE ui_button_get_prop(UI_WIDGET *widget, int prop) {
	struct UI_BUTTON_PROPERTIES *p=widget->properties;
	UI_PROPERTY_VALUE v={.p=NULL};
	switch(prop) {
		case UI_BUTTON_PROP_CHILD:
			v.p=p->child;
			break;
		case UI_BUTTON_PROP_ACTIVATED:
			v.i=p->activated;
			break;
		case UI_BUTTON_PROP_BORDER:
			v.p=p->border;
			break;
		case UI_BUTTON_PROP_ACTIVE_BORDER:
			v.p=p->active_border;
			break;
	}
	return v;
}

void ui_button_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	struct UI_BUTTON_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	p->child->resize(p->child, x+4+UI_PADDING, y+4+UI_PADDING, w-8-UI_PADDING*2, h-8-UI_PADDING*2);
	
	darnitRenderLineMove(p->border, 0, x, y, x+w-1, y);
	darnitRenderLineMove(p->border, 1, x, y+h, x+w-1, y+h);
	darnitRenderLineMove(p->border, 2, x, y, x, y+h-1);
	darnitRenderLineMove(p->border, 3, x+w, y, x+w, y+h-1);
	darnitRenderLineMove(p->border, 4, x+2, y+2, x+w-3, y+2);
	darnitRenderLineMove(p->border, 5, x+2, y+h-2, x+w-3, y+h-2);
	darnitRenderLineMove(p->border, 6, x+2, y+2, x+2, y+h-3);
	darnitRenderLineMove(p->border, 7, x+w-2, y+2, x+w-2, y+h-3);
	
	darnitRenderLineMove(p->active_border, 0, x, y+1, x+w-1, y+1);
	darnitRenderLineMove(p->active_border, 1, x, y+h-1, x+w-1, y+h-1);
	darnitRenderLineMove(p->active_border, 2, x+1, y, x+1, y+h-1);
	darnitRenderLineMove(p->active_border, 3, x+w-1, y, x+w-1, y+h-1);
}

void ui_button_request_size(UI_WIDGET *widget, int *w, int *h) {
	struct UI_BUTTON_PROPERTIES *p=widget->properties;
	p->child->request_size(p->child, w, h);
	(*w)+=8+UI_PADDING*2;
	(*h)+=8+UI_PADDING*2;
}

void ui_button_render(UI_WIDGET *widget) {
	struct UI_BUTTON_PROPERTIES *p=widget->properties;
	p->child->render(p->child);
	darnitRenderLineDraw(p->border, 8);
	if(p->activated)
		darnitRenderLineDraw(p->active_border, 4);
}