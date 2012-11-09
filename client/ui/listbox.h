#ifndef LISTBOX_H
#define LISTBOX_H

#define UI_LISTBOX_PROP_FONT 0
#define UI_LISTBOX_PROP_LIST 2
#define UI_LISTBOX_PROP_SCROLL_OFFSET 3
#define UI_LISTBOX_PROP_BORDER 4
#define UI_LISTBOX_PROP_SIZE 5
#define UI_LISTBOX_PROP_SCROLL 6
#define UI_LISTBOX_PROP_SELECTED 7
#define UI_LISTBOX_PROP_SELECTED_RECT 8

struct UI_LISTBOX_PROPERTIES {
	DARNIT_FONT *font;
	struct UI_LISTBOX_LIST *list;
	struct UI_LISTBOX_LIST *offset;
	DARNIT_LINE *border;
	int size;
	int scroll;
	int selected;
	DARNIT_RECT *selected_rect;
};

struct UI_LISTBOX_LIST {
	char text[128];
	DARNIT_TEXT_SURFACE *surface;
	struct UI_LISTBOX_LIST *next;
};

UI_WIDGET *ui_widget_create_listbox(DARNIT_FONT *font);
void *ui_widget_destroy_listbox(UI_WIDGET *widget);

void ui_listbox_event_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_listbox_add(UI_WIDGET *widget, char *text);
void ui_listbox_clear(UI_WIDGET *widget);
void ui_listbox_scroll(UI_WIDGET *widget, int pos);

void ui_listbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_listbox_get_prop(UI_WIDGET *widget, int prop);
void ui_listbox_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_listbox_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_listbox_render(UI_WIDGET *widget);

#endif
