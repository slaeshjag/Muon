#ifndef ENTRY_H
#define ENTRY_H

#define UI_ENTRY_LENGTH 256

#define UI_ENTRY_PROP_FONT 0
#define UI_ENTRY_PROP_SURFACE 1
#define UI_ENTRY_PROP_CURSOR_POS 2
#define UI_ENTRY_PROP_OFFSET 3
#define UI_ENTRY_PROP_BORDER 4
#define UI_ENTRY_PROP_CURSOR 5
#define UI_ENTRY_PROP_TEXT 5

struct UI_ENTRY_PROPERTIES{
	DARNIT_FONT *font;
	DARNIT_TEXT_SURFACE *surface;
	int cursor_pos;
	char *offset;
	DARNIT_LINE *border;
	DARNIT_LINE *cursor;
	char text[UI_ENTRY_LENGTH];
};

UI_WIDGET *ui_widget_create_entry(DARNIT_FONT *font);

void ui_entry_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void ui_entry_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void ui_entry_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_entry_get_prop(UI_WIDGET *widget, int prop);
void ui_entry_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_entry_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_entry_render(UI_WIDGET *widget);

#endif