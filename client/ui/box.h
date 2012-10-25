#ifndef BOX_H
#define BOX_H

typedef struct UI_WIDGET_LIST {
	UI_WIDGET *widget;
	int expand;
	struct UI_WIDGET_LIST *next;
} UI_WIDGET_LIST;

#endif