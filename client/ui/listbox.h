#ifndef LISTBOX_H
#define LISTBOX_H

#define UI_LISTBOX_PROP_FONT 0
#define UI_LISTBOX_PROP_LIST 2
#define UI_LISTBOX_PROP_SCROLL_OFFSET 3
#define UI_LISTBOX_PROP_BORDER 4
#define UI_LISTBOX_PROP_SIZE 5

struct UI_LISTBOX_PROPERTIES {
	DARNIT_FONT *font;
	struct UI_LISTBOX_LIST *list;
	struct UI_LISTBOX_LIST *offset;
	DARNIT_LINE *border;
	int size;
};

struct UI_LISTBOX_LIST {
	char text[128];
	DARNIT_TEXT_SURFACE *surface;
	struct UI_LISTBOX_LIST *next;
};

UI_WIDGET *ui_widget_create_listbox(DARNIT_FONT *font);

void ui_listbox_add(UI_WIDGET *widget, char *text);

void ui_listbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_listbox_get_prop(UI_WIDGET *widget, int prop);
void ui_listbox_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_listbox_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_listbox_render(UI_WIDGET *widget);

#endif
