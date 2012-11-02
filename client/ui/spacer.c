#include "ui.h"

UI_WIDGET *ui_widget_create_spacer() {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	
	widget->event_handler=NULL;
	widget->set_prop=ui_spacer_set_prop;
	widget->get_prop=ui_spacer_get_prop;
	widget->resize=ui_spacer_resize;
	widget->request_size=ui_spacer_request_size;
	widget->render=ui_spacer_render;
	widget->x=widget->y=widget->w=widget->h=0;
	
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
	if(w)
		*w=0;
	if(h)
		*h=0;
}

void ui_spacer_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
}

void ui_spacer_render(UI_WIDGET *widget) {
	return;
}
