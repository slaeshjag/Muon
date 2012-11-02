#ifndef UI_H
#define UI_H

#define UI_PADDING 4

//#define UI_LABEL(name) UI_WIDGET *(name)={ui_label_set_prop, ui_label_get_prop, NULL, ui_label_render}

#include "../muon.h"

typedef union {
	void *p;
	int i;
} UI_PROPERTY_VALUE;

typedef struct UI_WIDGET {
	void (*destroy)(struct UI_WIDGET *);
	void (*set_prop)(struct UI_WIDGET *, int, UI_PROPERTY_VALUE);
	UI_PROPERTY_VALUE (*get_prop)(struct UI_WIDGET *, int);
	/*void (*event)(struct UI_WIDGET *, int, int);*/
	void (*resize)(struct UI_WIDGET *, int, int, int, int);
	void (*request_size)(struct UI_WIDGET *, int *, int *);
	void (*render)(struct UI_WIDGET *);
	void *properties;
	struct UI_EVENT_HANDLER *event_handler;
	int x;
	int y;
	int w;
	int h;
} UI_WIDGET;

UI_WIDGET *ui_selected_widget;

#include "event.h"

#include "pane.h"
#include "label.h"
#include "vbox.h"
#include "hbox.h"
#include "button.h"
#include "checkbox.h"
#include "entry.h"
#include "progressbar.h"
#include "listbox.h"
#include "spacer.h"

void ui_init();

#endif
