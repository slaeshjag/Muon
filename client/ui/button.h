#ifndef BUTTON_H
#define BUTTON_H

#define UI_BUTTON_PROP_CHILD 0
#define UI_BUTTON_PROP_ACTIVATED 1
#define UI_BUTTON_PROP_BORDER 2
#define UI_BUTTON_PROP_ACTIVE_BORDER 3

struct UI_BUTTON_PROPERTIES {
	UI_WIDGET *child;
	int activated;
	DARNIT_LINE *border;
	DARNIT_LINE *active_border;
};

UI_WIDGET *ui_widget_create_button(UI_WIDGET *child);
UI_WIDGET *ui_widget_create_button_text(char *);
UI_WIDGET *ui_widget_create_button_image();

void ui_widget_destroy_button_text(UI_WIDGET *widget);

void ui_button_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void ui_button_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_button_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_button_get_prop(UI_WIDGET *widget, int prop);
void ui_button_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_button_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_button_render(UI_WIDGET *widget);

#endif
