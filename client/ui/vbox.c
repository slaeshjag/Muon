#include "ui.h"

UI_WIDGET *ui_widget_create_vbox() {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_VBOX_PROPERTIES)))==NULL) {
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
	widget->event_handler->add(widget, ui_vbox_event_notify_children, UI_EVENT_TYPE_MOUSE|UI_EVENT_TYPE_UI);
	
	struct UI_VBOX_PROPERTIES *p=widget->properties;
	p->size=0;
	p->children=NULL;
	
	widget->set_prop=ui_vbox_set_prop;
	widget->get_prop=ui_vbox_get_prop;
	widget->resize=ui_vbox_resize;
	widget->render=ui_vbox_render;
	widget->x=widget->y=widget->w=widget->h=0;
	
	return widget;
}

void ui_vbox_event_notify_children(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	struct UI_VBOX_PROPERTIES *p=widget->properties;
	UI_WIDGET_LIST *c;
	for(c=p->children; c; c=c->next)
		if(c->widget->event_handler) {
			if(PINR(e->mouse->x, e->mouse->y, c->widget->x, c->widget->y, c->widget->w, c->widget->h)) {
				if((type&UI_EVENT_TYPE_MOUSE_BUTTON))
					c->widget->event_handler->send(c->widget, type, e);
				else if(!PINR(ui_e_m_prev.x, ui_e_m_prev.y, c->widget->x, c->widget->y, c->widget->w, c->widget->h))
					c->widget->event_handler->send(c->widget, UI_EVENT_TYPE_MOUSE_ENTER, e);
			} else if((type==UI_EVENT_TYPE_MOUSE_LEAVE||type==UI_EVENT_TYPE_UI_EVENT)&&!PINR(ui_e_m_prev.x, ui_e_m_prev.y, c->widget->x, c->widget->y, c->widget->w, c->widget->h))
			c->widget->event_handler->send(c->widget, UI_EVENT_TYPE_MOUSE_LEAVE, e);
		}
}

void ui_vbox_add_child(UI_WIDGET *widget, UI_WIDGET *child, int expand) {
	struct UI_VBOX_PROPERTIES *p=widget->properties;
	UI_WIDGET_LIST **c;
	for(c=&(p->children); *c; c=&((*c)->next));
	if((*c=malloc(sizeof(UI_WIDGET_LIST)))==NULL)
		return;
	(*c)->next=NULL;
	(*c)->widget=child;
	(*c)->expand=expand;
	p->size++;
	widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
}

void ui_vbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	//struct UI_VBOX_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_VBOX_PROP_CHILDREN:
			break;
		case UI_VBOX_PROP_SIZE:
			break;
	}
}

UI_PROPERTY_VALUE ui_vbox_get_prop(UI_WIDGET *widget, int prop) {
	struct UI_VBOX_PROPERTIES *p=widget->properties;
	UI_PROPERTY_VALUE v={.p=NULL};
	
	switch(prop) {
		case UI_VBOX_PROP_CHILDREN:
			v.p=p->children;
			break;
		case UI_VBOX_PROP_SIZE:
			v.i=p->size;
			break;
	}
	return v;
}

void ui_vbox_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	if(w==0||h==0)
		return;
	struct UI_VBOX_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	if(p->size==0)
		return;
	int i, req_w, req_h, exp_h;
	int *requested=calloc(sizeof(int), p->size);
	UI_WIDGET_LIST *c;
	for(c=p->children, i=0; c; c=c->next) {
		if(!c->expand) {
			req_w=w; req_h=-1;
			c->widget->request_size(c->widget, &req_w, &req_h);
			requested[i]=req_h;
			i++;
			h-=req_h+UI_PADDING*2;
		}
	}
	if(p->size-i)
		exp_h=h/(p->size-i);
	else
		exp_h=0;
	for(c=p->children, i=0; c; c=c->next) {
		if(c->expand) {
			c->widget->resize(c->widget, x, y+UI_PADDING, w, exp_h);
			y+=exp_h;
		} else {
			c->widget->resize(c->widget, x, y+UI_PADDING, w, requested[i]-UI_PADDING*2);
			y+=requested[i];
			i++;
		}
	}
	free(requested);
}

void ui_vbox_render(UI_WIDGET *widget) {
	if(widget->w==0||widget->h==0)
		return;
	struct UI_VBOX_PROPERTIES *p=widget->properties;
	UI_WIDGET_LIST *c;
	for(c=p->children; c!=NULL&&c->widget!=NULL; c=c->next)
		c->widget->render(c->widget);
}