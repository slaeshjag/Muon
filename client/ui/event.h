#ifndef EVENT_H
#define EVENT_H

struct UI_PANE_LIST;

#include <SDL/SDL.h>
#define KEY(k) SDLK_##k

#define PINR(px, py, x, y, w, h) ((px)>(x)&&(py)>(y)&&(px)<((x)+(w))&&(py)<((y)+(h)))

#define UI_EVENT_TYPE_KEYBOARD		0x100
#define UI_EVENT_TYPE_KEYBOARD_PRESS	0x101
#define UI_EVENT_TYPE_KEYBOARD_RELEASE	0x102
#define UI_EVENT_TYPE_MOUSE		0x200
#define UI_EVENT_TYPE_MOUSE_BUTTON	0x21C
#define UI_EVENT_TYPE_MOUSE_ENTER	0x201
#define UI_EVENT_TYPE_MOUSE_LEAVE	0x202
#define UI_EVENT_TYPE_MOUSE_UP		0x204
#define UI_EVENT_TYPE_MOUSE_DOWN	0x208
#define UI_EVENT_TYPE_MOUSE_CLICK	0x210
#define UI_EVENT_TYPE_JOYSTICK		0x400
#define UI_EVENT_TYPE_UI		0x800
#define UI_EVENT_TYPE_UI_EVENT		0x801

#define UI_EVENT_KEYBOARD_MOD_CTRL	0x03
#define UI_EVENT_KEYBOARD_MOD_LCTRL	0x01
#define UI_EVENT_KEYBOARD_MOD_RCTRL	0x02
#define UI_EVENT_KEYBOARD_MOD_SHIFT	0x0C
#define UI_EVENT_KEYBOARD_MOD_LSHIFT	0x04
#define UI_EVENT_KEYBOARD_MOD_RSHIFT	0x08
#define UI_EVENT_KEYBOARD_MOD_ALT	0x30
#define UI_EVENT_KEYBOARD_MOD_LALT	0x10
#define UI_EVENT_KEYBOARD_MOD_RALT	0x20
#define UI_EVENT_KEYBOARD_MOD_SUPER	0xC0
#define UI_EVENT_KEYBOARD_MOD_LSUPER	0x40
#define UI_EVENT_KEYBOARD_MOD_RSUPER	0x80

typedef struct {
	unsigned char character;
	unsigned char modifiers;
	int keysym;
} UI_EVENT_KEYBOARD;

#define UI_EVENT_MOUSE_BUTTON_LEFT	0x01
#define UI_EVENT_MOUSE_BUTTON_MIDDLE	0x02
#define UI_EVENT_MOUSE_BUTTON_RIGHT	0x04

typedef struct {
	unsigned int x;
	unsigned int y;
	unsigned char buttons;
	signed int wheel;
} UI_EVENT_MOUSE;

typedef struct {
	
} UI_EVENT_UI;

typedef union {
	UI_EVENT_KEYBOARD *keyboard;
	UI_EVENT_MOUSE *mouse;
	/*UI_EVENT_JOYSTICK *joystick;*/
	UI_EVENT_UI *ui;
} UI_EVENT;

struct UI_EVENT_HANDLER_LIST {
	void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *);
	unsigned int mask;
	struct UI_EVENT_HANDLER_LIST *next;
};

typedef struct UI_EVENT_HANDLER {
	void (*add)(UI_WIDGET *, void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int);
	void (*remove)(UI_WIDGET *, void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *));
	void (*send)(UI_WIDGET *, unsigned int, UI_EVENT *);
	struct UI_EVENT_HANDLER_LIST *handlers;
} UI_EVENT_HANDLER;

UI_EVENT_MOUSE ui_e_m_prev;

void ui_events(struct UI_PANE_LIST *panes);
void ui_event_add(UI_WIDGET *widget, void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int mask);
void ui_event_send(UI_WIDGET *widget , unsigned int type, UI_EVENT *e);

#endif
