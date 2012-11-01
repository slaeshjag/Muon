#ifndef HBOX_H
#define HBOX_H

#include "box.h"

#define UI_HBOX_PROP_CHILDREN 0
#define UI_HBOX_PROP_SIZE 1

struct UI_HBOX_PROPERTIES {
	UI_WIDGET_LIST *children;
	int size;
};

UI_WIDGET *ui_widget_create_hbox();

void ui_hbox_event_notify_children(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_hbox_add_child(UI_WIDGET *widget, UI_WIDGET *child, int expand); 
void ui_hbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_hbox_get_prop(UI_WIDGET *widget, int prop);
void ui_hbox_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_hbox_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_hbox_render(UI_WIDGET *widget);

#endif
