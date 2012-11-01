#ifndef CHECKBOX_H
#define CHECKBOX_H

#define UI_CHECKBOX_PROP_ACTIVATED 0
#define UI_CHECKBOX_PROP_BORDER 1
#define UI_CHECKBOX_PROP_ACTIVE_BORDER 2

struct UI_CHECKBOX_PROPERTIES {
	int activated;
	DARNIT_LINE *border;
	DARNIT_LINE *active_border;
};

UI_WIDGET *ui_widget_create_checkbox();

void ui_checkbox_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void ui_checkbox_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_checkbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_checkbox_get_prop(UI_WIDGET *widget, int prop);
void ui_checkbox_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_checkbox_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_checkbox_render(UI_WIDGET *widget);

#endif
