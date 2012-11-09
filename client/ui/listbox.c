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
	p->border=darnitRenderLineAlloc(4, 1);
	p->size=0;
	p->scroll=0;
	p->selected=0;
	
	widget->destroy=ui_widget_destroy_listbox;
	widget->set_prop=ui_listbox_set_prop;
	widget->get_prop=ui_listbox_get_prop;
	widget->resize=ui_listbox_resize;
	widget->request_size=ui_listbox_request_size;
	widget->render=ui_listbox_render;
	widget->x=widget->y=widget->w=widget->h=0;
	return widget;
}

void *ui_widget_destroy_listbox(UI_WIDGET *widget) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	ui_listbox_clear(widget);
	darnitRenderLineFree(p->border);
	return ui_widget_destroy(widget);
}

void ui_listbox_add(UI_WIDGET *widget, char *text) {
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
		free(l);
	}
	p->list=NULL;
	p->offset=NULL;
	p->size=0;
	widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
}

void ui_listbox_scroll(UI_WIDGET *widget, int pos) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	int text_h;//darnitFontGetGlyphH(p->font)+2;
	struct UI_LISTBOX_LIST *l, *ll;
	int i=0;
	for(l=p->list; l; l=l->next) {
		text_h=2;
		for(ll=l; ll; ll=ll->next)
			text_h+=darnitTextStringGeometrics(p->font, ll->text, widget->w-UI_PADDING*2, NULL);
		if(pos>=0&&i>=pos-1)
			break;
		if(text_h<widget->h-2)
			break;
		i++;
	}
	p->offset=l;
	p->scroll=i+1;
	widget->resize(widget, widget->x, widget->y, widget->w, widget->h);
}

void ui_listbox_event_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	if(type==UI_EVENT_TYPE_MOUSE_SCROLL) {
		p->scroll+=e->mouse->wheel;
		p->scroll=p->scroll<0?0:p->scroll;
		ui_listbox_scroll(widget, p->scroll);
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
	}
	return v;
}

void ui_listbox_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	if(h==0||h==0)
		return;
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	
	darnitRenderLineMove(p->border, 0, x, y, x+w, y);
	darnitRenderLineMove(p->border, 1, x, y+h, x+w, y+h);
	darnitRenderLineMove(p->border, 2, x, y, x, y+h);
	darnitRenderLineMove(p->border, 3, x+w, y, x+w, y+h);
	
	if(!p->offset)
		p->offset=p->list;
	
	struct UI_LISTBOX_LIST *l=p->offset;
	int item_y=y+2;//, text_h=darnitFontGetGlyphH(p->font);
	/*for(l=p->offset; l&&item_y+darnitTextStringGeometrics(p->font, l->text, widget->w-UI_PADDING*2, NULL)<y+h-2; l=l->next) {
		if(l->surface)
			darnitTextSurfaceFree(l->surface);
		l->surface=darnitTextSurfaceAlloc(p->font, 128, w-UI_PADDING*2, x+UI_PADDING, item_y);
		darnitTextSurfaceStringAppend(l->surface, l->text);
		item_y+=darnitTextStringGeometrics(p->font, l->text, widget->w-UI_PADDING*2, NULL);
		printf("%s %i\n", l->text, item_y+darnitTextStringGeometrics(p->font, l->text, widget->w-UI_PADDING*2, NULL));
	}
	if(l&&l->next&&l->next->surface) {
		darnitTextSurfaceFree(l->next->surface);
		l->next->surface=NULL;
	}*/
	while(l) {
		l->surface=darnitTextSurfaceFree(l->surface);
		int ih=darnitTextStringGeometrics(p->font, l->text, w-UI_PADDING*2, NULL);
		if(item_y+ih>y+h-2)
			break;
		l->surface=darnitTextSurfaceAlloc(p->font, 128, w-UI_PADDING*2, x+UI_PADDING, item_y);
		darnitTextSurfaceStringAppend(l->surface, l->text);
		item_y+=ih;
		l=l->next;
	}
}

void ui_listbox_request_size(UI_WIDGET *widget, int *w, int *h) {
	if(w)
		*w=32;
	if(h)
		*h=32;
}

void ui_listbox_render(UI_WIDGET *widget) {
	struct UI_LISTBOX_PROPERTIES *p=widget->properties;
	darnitRenderLineDraw(p->border, 4);
	if(!p->offset)
		return;
	darnitRenderBlendingEnable();
	struct UI_LISTBOX_LIST *l;
	for(l=p->offset; l; l=l->next) {
		if(!l->surface)
			break;
		darnitTextSurfaceDraw(l->surface);
	}
	darnitRenderBlendingDisable();
}
