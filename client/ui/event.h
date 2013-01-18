/* Muon - a new kind of rts
 * Copyright 2012 Steven Arnow <s@rdw.se> and Axel Isaksson (mr3d/h4xxel)
 * 
 * This file is part of Muon.
 * 
 * Muon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Muon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Muon.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EVENT_H
#define EVENT_H

struct UI_PANE_LIST;

#include <SDL/SDL_keysym.h>
#define KEY(k) SDLK_##k

//Check if point (px, py) is inside the bounds of a rectangle
#define PINR(px, py, x, y, w, h) ((px)>(x)&&(py)>(y)&&(px)<((x)+(w))&&(py)<((y)+(h)))

#define UI_EVENT_TYPE_KEYBOARD		0x1FF
#define UI_EVENT_TYPE_KEYBOARD_PRESS	0x101
#define UI_EVENT_TYPE_KEYBOARD_RELEASE	0x102
#define UI_EVENT_TYPE_MOUSE		0x2FF
#define UI_EVENT_TYPE_MOUSE_BUTTON	0x20C
#define UI_EVENT_TYPE_MOUSE_ENTER	0x201
#define UI_EVENT_TYPE_MOUSE_LEAVE	0x202
#define UI_EVENT_TYPE_MOUSE_RELEASE		0x204
#define UI_EVENT_TYPE_MOUSE_PRESS	0x208
#define UI_EVENT_TYPE_MOUSE_DOWN	0x210
#define UI_EVENT_TYPE_MOUSE_SCROLL	0x220
#define UI_EVENT_TYPE_JOYSTICK		0x4FF
#define UI_EVENT_TYPE_BUTTONS		0x801
#define UI_EVENT_TYPE_UI		0x10FF
#define UI_EVENT_TYPE_UI_EVENT		0x1001
#define UI_EVENT_TYPE_UI_WIDGET_ACTIVATE	0x1002

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
	int character;
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
	
} UI_EVENT_JOYSTICK;

typedef struct {
	unsigned int	left	: 1;
	unsigned int	right	: 1;
	unsigned int	up	: 1;
	unsigned int	down	: 1;
	unsigned int	x	: 1;
	unsigned int	y	: 1;
	unsigned int	a	: 1;
	unsigned int	b	: 1;
	unsigned int	start	: 1;
	unsigned int	select	: 1;
	unsigned int	l	: 1;
	unsigned int	r	: 1;
	unsigned int	lmb	: 1;
	unsigned int	rmb	: 1;
	unsigned int	padding	: 2;
} UI_EVENT_BUTTONS;

typedef struct {
	
} UI_EVENT_UI;

typedef union {
	UI_EVENT_KEYBOARD *keyboard;
	UI_EVENT_MOUSE *mouse;
	UI_EVENT_JOYSTICK *joystick;
	UI_EVENT_BUTTONS *buttons;
	UI_EVENT_UI *ui;
} UI_EVENT;

struct UI_EVENT_HANDLER_LIST {
	void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *);
	unsigned int mask;
	struct UI_EVENT_HANDLER_LIST *next;
};

typedef struct UI_EVENT_HANDLER {
	void (*add)(UI_WIDGET *, void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int);
	void (*remove)(UI_WIDGET *, void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int);
	void (*send)(UI_WIDGET *, unsigned int, UI_EVENT *);
	struct UI_EVENT_HANDLER_LIST *handlers;
} UI_EVENT_HANDLER;

UI_EVENT_MOUSE ui_e_m_prev;

struct UI_EVENT_HANDLER_LIST *ui_event_global_handlers;

void ui_events(struct UI_PANE_LIST *panes, int render);
void ui_event_add(UI_WIDGET *widget, void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int mask);
void ui_event_remove(UI_WIDGET *widget, void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int mask);
void ui_event_send(UI_WIDGET *widget , unsigned int type, UI_EVENT *e);

void ui_event_global_add(void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int mask);
void ui_event_global_remove(void (*handler)(UI_WIDGET *, unsigned int, UI_EVENT *), unsigned int mask);
void ui_event_global_send(unsigned int type, UI_EVENT *e);

#endif
