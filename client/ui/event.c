#include "ui.h"

//higher value for lower speed, yeah it's stupid but whatever
#define KEYBOARD_REPEAT_SPEED 3
#define KEYBOARD_REPEAT_DELAY 20

#define SETMOD(m) e_k.modifiers=(key_action==DARNIT_KEYACTION_PRESS)?e_k.modifiers|UI_EVENT_KEYBOARD_MOD_##m :e_k.modifiers&~UI_EVENT_KEYBOARD_MOD_##m

void ui_events(struct UI_PANE_LIST *panes, int render) {
	UI_EVENT e;
	UI_EVENT_MOUSE e_m;
	UI_EVENT_UI e_u;
	static UI_EVENT_KEYBOARD e_k={0, 0, 0}, e_k_repeat={0, 0, 0};
	static int repeat=0;
	
	DARNIT_MOUSE mouse;
	mouse=darnitMouseGet();
	
	e_m.x=mouse.x; e_m.y=mouse.y;
	e_m.buttons=(mouse.lmb)|(mouse.rmb<<2);
	e_m.wheel=mouse.wheel;
	
	int key_action;
	e_k.keysym=darnitKeyboardRawPop(&key_action);
	switch(e_k.keysym) {
		case KEY(LCTRL):
			SETMOD(LCTRL);
			break;
		case KEY(RCTRL):
			SETMOD(RCTRL);
			break;
		case KEY(LSHIFT):
			SETMOD(LSHIFT);
			break;
		case KEY(RSHIFT):
			SETMOD(RSHIFT);
			break;
		case KEY(LALT):
			SETMOD(LALT);
			break;
		case KEY(RALT):
			SETMOD(RALT);
			break;
		case KEY(LSUPER):
			SETMOD(LSUPER);
			break;
		case KEY(RSUPER):
			SETMOD(RSUPER);
			break;
	}
	
	e_k.character=(e_k.keysym>=32&&e_k.keysym<127)?e_k.keysym-0x20*((e_k.modifiers&UI_EVENT_KEYBOARD_MOD_SHIFT)>0&&(e_k.keysym>=0x61&&e_k.keysym<0x7b)):0;
	
	e.keyboard=&e_k;
	if(ui_selected_widget) {
		if(e_k_repeat.character||e_k_repeat.keysym==8) {
			if(repeat>KEYBOARD_REPEAT_DELAY&&!((repeat-KEYBOARD_REPEAT_DELAY)%KEYBOARD_REPEAT_SPEED)) {
				UI_EVENT e_repeat={.keyboard=&e_k_repeat};
				ui_selected_widget->event_handler->send(ui_selected_widget, UI_EVENT_TYPE_KEYBOARD_PRESS, &e_repeat);
			}
			repeat++;
		}
		if(key_action==DARNIT_KEYACTION_PRESS) {
			if(e_k.character||e_k.keysym==8) {
				e_k_repeat=e_k;
				repeat=0;
			}
			ui_selected_widget->event_handler->send(ui_selected_widget, UI_EVENT_TYPE_KEYBOARD_PRESS, &e);
		} else if(key_action==DARNIT_KEYACTION_RELEASE) {
			if(e_k.keysym==e_k_repeat.keysym) {
				e_k_repeat.character=0; e_k_repeat.keysym=0; e_k_repeat.modifiers=0;
				repeat=0;
			}
			ui_selected_widget->event_handler->send(ui_selected_widget, UI_EVENT_TYPE_KEYBOARD_RELEASE, &e);
		}
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
		if(render)
			ui_pane_render(p->pane);
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
