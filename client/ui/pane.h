#ifndef PANE_H
#define PANE_H

typedef struct {
	int x;
	int y;
	int w;
	int h;
	DARNIT_LINE *border;
	DARNIT_RECT *background;
	UI_WIDGET *root_widget;
	UI_EVENT_HANDLER *event_handler;
} UI_PANE;

struct UI_PANE_LIST {
	UI_PANE *pane;
	struct UI_PANE_LIST *next;
};

UI_PANE *ui_pane_create(int x, int y, int w, int h, UI_WIDGET *root_widget);
void ui_pane_destroy(UI_PANE *pane);
void ui_pane_resize(UI_PANE *pane, int x, int y, int w, int h);
void ui_pane_set_root_widget(UI_PANE *pane, UI_WIDGET *root_widget);
void ui_pane_render(UI_PANE *pane);

#endif