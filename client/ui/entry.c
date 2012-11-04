#include <string.h>

#include "ui.h"

UI_WIDGET *ui_widget_create_entry(DARNIT_FONT *font) {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_ENTRY_PROPERTIES)))==NULL) {
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
	widget->event_handler->add(widget, ui_entry_event_click, UI_EVENT_TYPE_MOUSE);
	widget->event_handler->add(widget, ui_entry_event_key, UI_EVENT_TYPE_KEYBOARD);
	
	struct UI_ENTRY_PROPERTIES *p=widget->properties;
	p->surface=NULL;
	p->font=font;
	memset(p->text, 0, UI_ENTRY_LENGTH+1);
	p->offset=p->text;
	p->cursor_pos=0;
	p->cursor=0;
	p->border=darnitRenderLineAlloc(4, 1);
	
	widget->set_prop=ui_entry_set_prop;
	widget->get_prop=ui_entry_get_prop;
	widget->resize=ui_entry_resize;
	widget->request_size=ui_entry_request_size;
	widget->render=ui_entry_render;
	widget->x=widget->y=widget->w=widget->h=0;

	return widget;
}

void ui_entry_event_key(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	struct UI_ENTRY_PROPERTIES *p=widget->properties;
	switch(type) {
		case UI_EVENT_TYPE_KEYBOARD_PRESS:
			if(e->keyboard->keysym==8&&p->cursor_pos>0) {
				p->cursor_pos--;
				p->text[p->cursor_pos]=0;
				if(p->offset>p->text+1) {
					for(; darnitFontGetStringWidthPixels(p->font, p->offset)<widget->w; p->offset--);
					p->offset++;
				} else if(p->offset==p->text+1)
					p->offset=p->text;
			} else if(p->cursor_pos>=UI_ENTRY_LENGTH-1||!e->keyboard->character) {
				return;
			} else {
				p->text[p->cursor_pos]=e->keyboard->character;
				for(; darnitFontGetStringWidthPixels(p->font, p->offset)>widget->w; p->offset++);
				p->cursor_pos++;
			}
			darnitTextSurfaceReset(p->surface);
			darnitTextSurfaceStringAppend(p->surface, p->offset);
			//darnitTextSurfaceCharAppend(p->surface, &e->keyboard->character);
			break;
	}
}

void ui_entry_event_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	switch(type) {
		case UI_EVENT_TYPE_MOUSE_DOWN:
			if(e->mouse->buttons==UI_EVENT_MOUSE_BUTTON_LEFT)
				ui_selected_widget=widget;
			break;
	}
}

void ui_entry_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_ENTRY_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_ENTRY_PROP_TEXT:
			strncpy(p->text, value.p, UI_ENTRY_LENGTH);
			p->offset=p->text;
			for(; darnitFontGetStringWidthPixels(p->font, p->offset)>widget->w; p->offset++);
			p->cursor_pos=strlen(p->text);
			darnitTextSurfaceReset(p->surface);
			darnitTextSurfaceStringAppend(p->surface, p->offset);
			break;
	}
}

UI_PROPERTY_VALUE ui_entry_get_prop(UI_WIDGET *widget, int prop) {
	UI_PROPERTY_VALUE v={.p=NULL};
	struct UI_ENTRY_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_ENTRY_PROP_FONT:
			v.p=p->font;
			break;
		case UI_ENTRY_PROP_SURFACE:
			v.p=p->surface;
			break;
		case UI_ENTRY_PROP_TEXT:
			v.p=p->text;
			break;
	}
	return v;
}

void ui_entry_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	if(h==0||h==0)
		return;
	struct UI_ENTRY_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	
	darnitRenderLineMove(p->border, 0, x, y, x+w, y);
	darnitRenderLineMove(p->border, 1, x, y+h, x+w, y+h);
	darnitRenderLineMove(p->border, 2, x, y, x, y+h);
	darnitRenderLineMove(p->border, 3, x+w, y, x+w, y+h);
	
	if(p->surface!=NULL)
		darnitTextSurfaceFree(p->surface);
	int text_h=darnitFontGetGlyphH(p->font);//darnitTextStringGeometrics(p->font, p->offset, w, &text_w);
	p->surface=darnitTextSurfaceAlloc(p->font, UI_ENTRY_LENGTH, w, x+2, y+(h/2)-(text_h/2));
	darnitTextSurfaceStringAppend(p->surface, p->offset);
}

void ui_entry_request_size(UI_WIDGET *widget, int *w, int *h) {
	int ww=64+4;
	if(w)
		*w=ww;
	if(!h)
		return;
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	int text_h=darnitFontGetGlyphH(p->font);
	//darnitTextStringGeometrics(p->font, p->offset, ww, &text_w);
	*h=text_h+4;
}

void ui_entry_render(UI_WIDGET *widget) {
	struct UI_ENTRY_PROPERTIES *p=widget->properties;
	darnitRenderLineDraw(p->border, 4);
	
	darnitRenderBlendingEnable();
	darnitTextSurfaceDraw(p->surface);
	darnitRenderBlendingDisable();
}
