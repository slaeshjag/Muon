#include "ui.h"

void ui_init() {
	ui_selected_widget=NULL;
	ui_e_m_prev.x=ui_e_m_prev.y=ui_e_m_prev.buttons=ui_e_m_prev.wheel=0;
}

void *ui_widget_destroy(UI_WIDGET *widget) {
	struct UI_EVENT_HANDLER_LIST *h, *next;
	for(h=widget->event_handler->handlers; h; h=next) {
		next=h->next;
		free(h);
	}
	free(widget->event_handler);
	free(widget->properties);
	free(widget);
	return NULL;
}
