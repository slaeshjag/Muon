#include "ui.h"

void ui_events(struct UI_PANE_LIST *panes) {
	UI_EVENT e;
	UI_EVENT_MOUSE e_m;
	//UI_EVENT_KEYBOARD e_k;
	
	static UI_EVENT_MOUSE oldmouse={0};
	DARNIT_MOUSE mouse;
	mouse=darnitMouseGet();
	
	e_m.x=mouse.x; e_m.y=mouse.y;
	e_m.buttons=(mouse.lmb)|(mouse.rmb<<2);
	e_m.wheel=mouse.wheel;
	
	struct UI_PANE_LIST *p;
	for(p=panes; p; p=p->next) {
		if(p->pane->root_widget->event_handler) {
			UI_WIDGET *w=p->pane->root_widget;
			
			e.mouse=&e_m;
			if(PINR(e_m.x, e_m.y, w->x, w->y, w->w, w->h)) {
				if(!PINR(oldmouse.x, oldmouse.y, w->x, w->y, w->w, w->h))
					w->event_handler->send(w, UI_EVENT_TYPE_MOUSE_ENTER, &e);
				if((oldmouse.buttons&e_m.buttons)<e_m.buttons)
					w->event_handler->send(w, UI_EVENT_TYPE_MOUSE_DOWN, &e);
				if((oldmouse.buttons&e_m.buttons)<oldmouse.buttons)
					w->event_handler->send(w, UI_EVENT_TYPE_MOUSE_UP, &e);
				
			} else if(PINR(oldmouse.x, oldmouse.y, w->x, w->y, w->w, w->h))
				w->event_handler->send(w, UI_EVENT_TYPE_MOUSE_LEAVE, &e);
		}
	}
	oldmouse=e_m;
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
			h->handler(widget, type&h->mask, e);
}