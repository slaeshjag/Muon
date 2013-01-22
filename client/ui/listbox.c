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

#include <string.h>

#include "ui.h"

UI_WIDGET *ui_widget_create_listbox(DARNIT_FONT *font) {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_LISTBOX_PROPERTIES)))==NULL) {
		free(widget);
		return NULL;
	}
	if((widget->event_handler=malloc(sizeof(UI_EVENT_HANDLER)))==NULL) {
		free(widget->properties);
		free(widget);
		return NULL;
	}
	widget->event_handler->handlers=NULL;
	widget->event_handler->add=ui_event_add;
	widget->event_handler->remove=ui_event_remove;
	widget->event_handler->send=ui_event_send;
	widget->event_handler->add(widget, ui_listbox_event_mouse, UI_EVENT_TYPE_MOUSE);
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	p->list=NULL;
	p->offset=NULL;
	p->font=font;
	p->border=d_render_line_new(4, 1);
	p->selected_rect=d_render_rect_new(1);
	p->scrollbar=d_render_rect_new(1);
	p->size=0;
	p->scroll=0;
	p->scroll_max=0;
	p->selected=-1;
	
	widget->destroy=ui_widget_destroy_listbox;
	widget->set_prop=ui_listbox_set_prop;
	widget->get_prop=ui_listbox_get_prop;
	widget->resize=ui_listbox_resize;
	widget->request_size=ui_listbox_request_size;
	widget->render=ui_listbox_render;
	widget->x=widget->y=widget->w=widget->h=0;
	widget->enabled=1;
	
	return widget;
}

void *ui_widget_destroy_listbox(UI_WIDGET *widget) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	ui_listbox_clear(widget);
	d_render_line_free(p->border);
	d_render_rect_free(p->selected_rect);
	d_render_rect_free(p->scrollbar);
	return ui_widget_destroy(widget);
}

void ui_listbox_add(UI_WIDGET *widget, const char *text) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	struct UI_LISTBOX_LIST **l;
	for(l=&(p->list); *l; l=&((*l)->next));
	if((*l=malloc(sizeof(struct UI_LISTBOX_LIST)))==NULL)
		return;
	(*l)->next=NULL;
	(*l)->surface=NULL;
	strncpy((*l)->text, text, 127);
	(*l)->text[127]=0;
	p->size++;
	widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
}

void ui_listbox_clear(UI_WIDGET *widget) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	struct UI_LISTBOX_LIST *l, *next;
	for(l=p->list; l; l=next) {
		next=l->next;
		d_text_surface_free(l->surface);
		free(l);
	}
	p->list=NULL;
	p->offset=NULL;
	p->size=0;
	p->scroll=0;
	p->selected=-1;
	widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
}

char *ui_listbox_get(UI_WIDGET *widget, int index) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	struct UI_LISTBOX_LIST *l;
	int i;
	for(l=p->list, i=0; l->next&&i<index; l=l->next, i++);
	return l->text;
}

void ui_listbox_set(UI_WIDGET *widget, int index, const char *text) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	struct UI_LISTBOX_LIST *l;
	int i;
	for(l=p->list, i=0; l->next&&i<index; l=l->next, i++);
	strcpy(l->text, text);
	widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
}

int ui_listbox_index_of(UI_WIDGET *widget, const char *text) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	struct UI_LISTBOX_LIST *l;
	int i;
	for(l=p->list, i=0; l; l=l->next, i++)
		if(!strcmp(text, l->text))
			return i;
	return -1;
}

void ui_listbox_scroll(UI_WIDGET *widget, int pos) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	int text_h;
	struct UI_LISTBOX_LIST *l, *ll;
	int i=0;
	for(l=p->list; l; l=l->next) {
		text_h=2;
		for(ll=l; ll; ll=ll->next)
			text_h+=d_font_string_geometrics(p->font, ll->text, widget->w-UI_PADDING*5, NULL);
		if((pos>=0&&i>=pos))
			break;
		if(text_h<widget->h-2)
			break;
		i++;
	}
	p->offset=l;
	p->scroll=i;
	
	pos=-1;
	for(; l; l=l->next) {
		text_h=2;
		for(ll=l; ll; ll=ll->next)
			text_h+=d_font_string_geometrics(p->font, ll->text, widget->w-UI_PADDING*5, NULL);
		if(text_h<widget->h-2)
			break;
		i++;
	}
	p->scroll_max=i;	
	widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
}

void ui_listbox_event_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	static int scroll_y=-1;
	if(!widget->enabled)
		return;
	if(type==UI_EVENT_TYPE_MOUSE_SCROLL) {
		p->scroll+=e->mouse->wheel;
		p->scroll=p->scroll<0?0:p->scroll;
		ui_listbox_scroll(widget, p->scroll);
	} else if(e->mouse->x>=widget->x+widget->w-UI_PADDING*3&&e->mouse->x<widget->x+widget->w-UI_PADDING) {
		if(type==UI_EVENT_TYPE_MOUSE_PRESS) {
			int sb_y1, sb_y2;
			d_render_rect_get(p->scrollbar, 0, NULL, &sb_y1, NULL, &sb_y2);
			if(e->mouse->y<sb_y1) {
				p->scroll--;
				p->scroll=p->scroll<0?0:p->scroll;
				ui_listbox_scroll(widget, p->scroll-1);
				scroll_y=-1;
			} else if(e->mouse->y>=sb_y2) {
				p->scroll++;
				ui_listbox_scroll(widget, p->scroll);
				scroll_y=-1;
			} else
				scroll_y=e->mouse->y-sb_y1;
			
		} else if(type==UI_EVENT_TYPE_MOUSE_DOWN&&e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_LEFT&&scroll_y>-1) {
			int sb_y1, sb_y2;
			d_render_rect_get(p->scrollbar, 0, NULL, &sb_y1, NULL, &sb_y2);
			int delta=e->mouse->y-(widget->y+UI_PADDING/2)-scroll_y;
			int scroll=p->scroll_max*(delta<0?0:delta)/(widget->h-UI_PADDING-(sb_y2-sb_y1));
			ui_listbox_scroll(widget, scroll<0?0:scroll);
			return;
		}
	} else if(type==UI_EVENT_TYPE_MOUSE_PRESS&&e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_LEFT) {
		struct UI_LISTBOX_LIST *l;
		int i=p->scroll;
		int item_y=widget->y+2;
		for(l=p->offset; l; l=l->next, i++) {
			int item_h=d_font_string_geometrics(p->font, l->text, widget->w-UI_PADDING*5, NULL);
			if(e->mouse->y>item_y&&e->mouse->y<item_y+item_h) {
				p->selected=i;
				widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
				widget->event_handler->send(widget, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE, NULL);
				break;
			}
			item_y+=item_h;
		}
	}
}

void ui_listbox_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_LISTBOX_PROP_FONT:
			p->font=value.p;
			break;
	}
}

UI_PROPERTY_VALUE ui_listbox_get_prop(UI_WIDGET *widget, int prop) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	UI_PROPERTY_VALUE v={.p=NULL};
	switch(prop) {
		case UI_LISTBOX_PROP_FONT:
			v.p=p->font;
			break;
		case UI_LISTBOX_PROP_SIZE:
			v.i=p->size;
			break;
		case UI_LISTBOX_PROP_SCROLL:
			v.i=p->scroll;
			break;
		case UI_LISTBOX_PROP_SELECTED:
			v.i=p->selected;
			break;
	}
	return v;
}

void ui_listbox_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	if(h==0||h==0)
		return;
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	
	d_render_line_move(p->border, 0, x, y, x+w, y);
	d_render_line_move(p->border, 1, x, y+h, x+w, y+h);
	d_render_line_move(p->border, 2, x, y, x, y+h);
	d_render_line_move(p->border, 3, x+w, y, x+w, y+h);
	
	if(!p->offset)
		p->offset=p->list;
	
	struct UI_LISTBOX_LIST *l=p->offset;
	int item_y=y+2;
	int i=p->scroll;
	while(l) {
		l->surface=d_text_surface_free(l->surface);
		int item_h=d_font_string_geometrics(p->font, l->text, w-UI_PADDING*5, NULL);
		if(item_y+item_h>y+h-2)
			break;
		l->surface=d_text_surface_new(p->font, 128, w-UI_PADDING*4, x+UI_PADDING, item_y);
		d_text_surface_string_append(l->surface, l->text);
		if(i==p->selected)
			d_render_rect_move(p->selected_rect, 0, x+UI_PADDING, item_y, x+w-UI_PADDING*3, item_y+item_h);
		item_y+=item_h;
		l=l->next; i++;
	}
	int scroll=p->size?(widget->h-UI_PADDING)*p->scroll/p->size:0;
	int scroll_left=p->size?(widget->h-UI_PADDING)*(p->scroll_max)/p->size:0;
	d_render_rect_move(p->scrollbar, 0, widget->x+widget->w-(UI_PADDING*2+UI_PADDING/2), widget->y+UI_PADDING/2+scroll, widget->x+widget->w-UI_PADDING, widget->y+widget->h-UI_PADDING/2-scroll_left+scroll);
}

void ui_listbox_request_size(UI_WIDGET *widget, int *w, int *h) {
	if(w)
		*w=128;
	if(h)
		*h=64;
}

void ui_listbox_render(UI_WIDGET *widget) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	d_render_line_draw(p->border, 4);
	if(!p->offset)
		return;
	d_render_blend_enable();
	struct UI_LISTBOX_LIST *l;
	int i=p->scroll;
	for(l=p->offset; l; l=l->next, i++) {
		if(!l->surface)
			break;
		d_text_surface_draw(l->surface);
	}
	d_render_blend_disable();
	if(p->selected>=p->scroll&&p->selected<i) {
		d_render_logic_op(DARNIT_RENDER_LOGIC_OP_XOR);
		d_render_rect_draw(p->selected_rect, 1);
		d_render_logic_op(DARNIT_RENDER_LOGIC_OP_NONE);
	}
	d_render_rect_draw(p->scrollbar, 1);
}
