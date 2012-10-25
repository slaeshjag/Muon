#ifndef LABEL_H
#define LABEL_H

#define UI_LABEL_PROP_FONT 0
#define UI_LABEL_PROP_SURFACE 1
#define UI_LABEL_PROP_TEXT 2

struct UI_LABEL_PROPERTIES{
	DARNIT_FONT *font;
	DARNIT_TEXT_SURFACE *surface;
	char *text;
};

UI_WIDGET *ui_widget_create_label();
void ui_label_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_label_get_prop(UI_WIDGET *widget, int prop);
void ui_label_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_label_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_label_render(UI_WIDGET *widget);

#endif