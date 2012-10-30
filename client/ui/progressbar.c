#include <string.h>

#include "ui.h"

UI_WIDGET *ui_widget_create_progressbar(DARNIT_FONT *font) {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_PROGRESSBAR_PROPERTIES)))==NULL) {
		free(widget);
		return NULL;
	}
	widget->event_handler=NULL;
	
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	p->surface=NULL;
	p->font=font;
	p->border=darnitRenderLineAlloc(4, 1);
	p->bar=darnitRenderRectAlloc(1);
	p->progress=0;
	strcpy(p->text, "0%");
	
	widget->set_prop=ui_progressbar_set_prop;
	widget->get_prop=ui_progressbar_get_prop;
	widget->resize=ui_progressbar_resize;
	widget->request_size=ui_progressbar_request_size;
	widget->render=ui_progressbar_render;
	widget->x=widget->y=widget->w=widget->h=0;

	return widget;
}

void ui_progressbar_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_PROGRESSBAR_PROP_PROGRESS:
			if(value.i<0||value.i>100)
				break;
			sprintf(p->text, "%i%%", value.i);
			darnitRenderRectSet(p->bar, 0, widget->x+2, widget->y+2, widget->x+(widget->w*value.i/100)-2, widget->y+widget->h-2);
			if(p->surface!=NULL)
				darnitTextSurfaceFree(p->surface);
			int text_w;
			int text_h=darnitTextStringGeometrics(p->font, p->text, widget->w, &text_w);
			p->surface=darnitTextSurfaceAlloc(p->font, darnitUtf8UnicodeCharactersInString(p->text), widget->w, widget->x+(widget->w/2)-(text_w/2), widget->y+(widget->h/2)-(text_h/2));
			darnitTextSurfaceStringAppend(p->surface, p->text);
			break;
	}
}

UI_PROPERTY_VALUE ui_progressbar_get_prop(UI_WIDGET *widget, int prop) {
	UI_PROPERTY_VALUE v={.p=NULL};
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_PROGRESSBAR_PROP_FONT:
			v.p=p->font;
			break;
		case UI_PROGRESSBAR_PROP_SURFACE:
			v.p=p->surface;
			break;
		case UI_PROGRESSBAR_PROP_TEXT:
			v.p=p->text;
			break;
	}
	return v;
}

void ui_progressbar_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	if(h==0||h==0)
		return;
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	
	darnitRenderLineMove(p->border, 0, x, y, x+w, y);
	darnitRenderLineMove(p->border, 1, x, y+h, x+w, y+h);
	darnitRenderLineMove(p->border, 2, x, y, x, y+h);
	darnitRenderLineMove(p->border, 3, x+w, y, x+w, y+h);
	
	darnitRenderRectSet(p->bar, 0, widget->x+2, widget->y+2, widget->x+widget->w-2, widget->y+widget->h-2);
	
	if(p->surface!=NULL)
		darnitTextSurfaceFree(p->surface);
	int text_w;
	int text_h=darnitTextStringGeometrics(p->font, p->text, w, &text_w);
	p->surface=darnitTextSurfaceAlloc(p->font, darnitUtf8UnicodeCharactersInString(p->text), w, x+(w/2)-(text_w/2), y+(h/2)-(text_h/2));
	darnitTextSurfaceStringAppend(p->surface, p->text);
}

void ui_progressbar_request_size(UI_WIDGET *widget, int *w, int *h) {
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

void ui_progressbar_render(UI_WIDGET *widget) {
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	darnitRenderLineDraw(p->border, 4);
	darnitRenderRectDraw(p->bar, 1);
	
	darnitRenderBlendingEnable();
	darnitTextSurfaceDraw(p->surface);
	darnitRenderBlendingDisable();
}
