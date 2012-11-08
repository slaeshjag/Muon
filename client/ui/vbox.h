#ifndef VBOX_H
#define VBOX_H

#include "box.h"

#define UI_VBOX_PROP_CHILDREN 0
#define UI_VBOX_PROP_SIZE 1

struct UI_VBOX_PROPERTIES {
	UI_WIDGET_LIST *children;
	int size;
};

UI_WIDGET *ui_widget_create_vbox();
void *ui_widget_vbox_destroy(UI_WIDGET* widget);

void ui_vbox_event_notify_children(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_vbox_add_child(UI_WIDGET *widget, UI_WIDGET *child, int expand);
void ui_vbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_vbox_get_prop(UI_WIDGET *widget, int prop);
void ui_vbox_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_vbox_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_vbox_render(UI_WIDGET *widget);

#endif
