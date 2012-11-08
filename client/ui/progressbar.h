#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#define UI_PROGRESSBAR_PROP_FONT 0
#define UI_PROGRESSBAR_PROP_SURFACE 1
#define UI_PROGRESSBAR_PROP_BORDER 2
#define UI_PROGRESSBAR_PROP_BAR 3
#define UI_PROGRESSBAR_PROP_PROGRESS 4
#define UI_PROGRESSBAR_PROP_TEXT 5

struct UI_PROGRESSBAR_PROPERTIES {
	DARNIT_FONT *font;
	DARNIT_TEXT_SURFACE *surface;
	DARNIT_LINE *border;
	DARNIT_RECT *bar;
	int progress;
	char text[5];
};

UI_WIDGET *ui_widget_create_progressbar(DARNIT_FONT *font);
void *ui_widget_destroy_progressbar(UI_WIDGET *widget);

void ui_progressbar_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
UI_PROPERTY_VALUE ui_progressbar_get_prop(UI_WIDGET *widget, int prop);
void ui_progressbar_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_progressbar_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_progressbar_render(UI_WIDGET *widget);

#endif
