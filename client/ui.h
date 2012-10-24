#ifndef UI_H
#define UI_H

//#define UI_LABEL(name) UI_WIDGET *(name)={ui_label_set_prop, ui_label_get_prop, NULL, ui_label_render}

#include "muon.h"

typedef union {
	void *p;
	int i;
} UI_PROPERTY_VALUE;

typedef struct UI_WIDGET {
	void (*set_prop)(struct UI_WIDGET *, int, UI_PROPERTY_VALUE);
	UI_PROPERTY_VALUE (*get_prop)(struct UI_WIDGET *, int);
	void (*event)(struct UI_WIDGET *, int, int);
	void (*render)(struct UI_WIDGET *, int, int, int, int);
	void *properties;
} UI_WIDGET;

typedef struct {
	UI_WIDGET *widget;
	struct UI_WIDGET_LIST *next;
} UI_WIDGET_LIST;

typedef struct {
	int x;
	int y;
	int w;
	int h;
	DARNIT_LINE *border;
	UI_WIDGET *root_widget;
} UI_PANE;

#define UI_LABEL_PROP_FONT 0
#define UI_LABEL_PROP_TEXT 1

struct UI_LABEL_PROPERTIES{
	DARNIT_FONT *font;
	char *text;
};

void ui_widget_destroy();

UI_PANE *ui_pane_create(int x, int y, int w, int h, UI_WIDGET *root_widget);
void ui_pane_destroy(UI_PANE *pane);
void ui_pane_resize(UI_PANE *pane, int x, int y, int w, int h);
void ui_pane_set_root_widget(UI_PANE *pane, UI_WIDGET *root_widget);
void ui_pane_render(UI_PANE *pane);

UI_WIDGET *ui_widget_create_label();
void ui_label_set_prop(UI_WIDGET *, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_label_get_prop(UI_WIDGET *, int prop);
void ui_label_render(UI_WIDGET *, int x, int y, int w, int h);

#endif