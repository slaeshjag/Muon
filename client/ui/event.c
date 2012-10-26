#include "ui.h"

void ui_events(struct UI_PANE_LIST *panes) {
	UI_EVENT e;
	UI_EVENT_MOUSE e_m;
	UI_EVENT_UI e_u;
	UI_EVENT_KEYBOARD e_k;
	
	DARNIT_MOUSE mouse;
	mouse=darnitMouseGet();
	
	e_m.x=mouse.x; e_m.y=mouse.y;
	e_m.buttons=(mouse.lmb)|(mouse.rmb<<2);
	e_m.wheel=mouse.wheel;
	
	int key_action;
	e_k.keysym=darnitKeyboardRawPop(&key_action);
	
	e.keyboard=&e_k;
	if(ui_selected_widget) {
		if(key_action==DARNIT_KEYACTION_PRESS)
			ui_selected_widget->event_handler->send(ui_selected_widget, UI_EVENT_TYPE_KEYBOARD_PRESS, &e);
		else if(key_action==DARNIT_KEYACTION_RELEASE)
			ui_selected_widget->event_handler->send(ui_selected_widget, UI_EVENT_TYPE_KEYBOARD_RELEASE, &e);
	}
	
	struct UI_PANE_LIST *p;
	for(p=panes; p; p=p->next) {
		if(p->pane->root_widget->event_handler) {
			UI_WIDGET *w=p->pane->root_widget;
			
			e.ui=&e_u;
			e.mouse=&e_m; //yep, ugly hack here
			w->event_handler->send(w, UI_EVENT_TYPE_UI_EVENT, &e);
			
			if(PINR(e_m.x, e_m.y, w->x, w->y, w->w, w->h)) {
				if(!PINR(ui_e_m_prev.x, ui_e_m_prev.y, w->x, w->y, w->w, w->h))
					w->event_handler->send(w, UI_EVENT_TYPE_MOUSE_ENTER, &e);
				if((ui_e_m_prev.buttons&e_m.buttons)<e_m.buttons)
					w->event_handler->send(w, UI_EVENT_TYPE_MOUSE_DOWN, &e);
				if((ui_e_m_prev.buttons&e_m.buttons)<ui_e_m_prev.buttons)
					w->event_handler->send(w, UI_EVENT_TYPE_MOUSE_UP, &e);
				
			} else if(PINR(ui_e_m_prev.x, ui_e_m_prev.y, w->x, w->y, w->w, w->h))
				w->event_handler->send(w, UI_EVENT_TYPE_MOUSE_LEAVE, &e);
		}
	}
	ui_e_m_prev=e_m;
}

void ui_event_add(UI_WIDGET *widget, void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int mask) {
	struct UI_EVENT_HANDLER_LIST **h, *hh;
	for(h=&(widget->event_handler->handlers); *h; h=&((*h)->next));
	if((*h=malloc(sizeof(struct UI_EVENT_HANDLER_LIST)))==NULL)
		return;
	hh=*h;
	hh->handler=handler;
	hh->mask=mask;
	hh->next=NULL;
}

void ui_event_send(UI_WIDGET *widget , unsigned int type, UI_EVENT *e) {
	struct UI_EVENT_HANDLER_LIST *h;
	for(h=widget->event_handler->handlers; h; h=h->next)
		if(type&h->mask)
			h->handler(widget, type&(h->mask|0xFF), e);
}