#include "ui.h"

UI_WIDGET *ui_widget_create_spacer() {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_SPACER_PROPERTIES)))==NULL) {
		free(widget);
		return NULL;
	}
	struct UI_SPACER_PROPERTIES *p=widget->properties;
	p->set_w=0;
	p->set_h=0;
	
	widget->event_handler=NULL;
	widget->destroy=ui_widget_destroy;
	widget->set_prop=ui_spacer_set_prop;
	widget->get_prop=ui_spacer_get_prop;
	widget->resize=ui_spacer_resize;
	widget->request_size=ui_spacer_request_size;
	widget->render=ui_spacer_render;
	widget->x=widget->y=widget->w=widget->h=0;
	
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
