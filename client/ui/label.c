#include "ui.h"

UI_WIDGET *ui_widget_create_label(DARNIT_FONT *font, char *text) {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_LABEL_PROPERTIES)))==NULL) {
		free(widget);
		return NULL;
	}
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	p->surface=NULL;
	p->font=font;
	widget->event_handler=NULL;
	widget->set_prop=ui_label_set_prop;
	widget->get_prop=ui_label_get_prop;
	widget->resize=ui_label_resize;
	widget->request_size=ui_label_request_size;
	widget->render=ui_label_render;
	widget->x=widget->y=widget->w=widget->h=0;
	//v.p=darnitTextSurfaceAlloc(font, darnitUtf8UnicodeCharactersInString(text), widget->w, widget->x, widget->y);
	//widget->set_prop(widget, UI_LABEL_PROP_SURFACE, v);
	UI_PROPERTY_VALUE v={.p=text};
	widget->set_prop(widget, UI_LABEL_PROP_TEXT, v);
	return widget;
}

void ui_label_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_LABEL_PROP_TEXT:
			p->text=value.p;
			ui_label_resize(widget, widget->x, widget->y, widget->w, widget->h);
			break;
		/*case UI_LABEL_PROP_SURFACE:
			p->text=value.p;
			break;*/
		case UI_LABEL_PROP_FONT:
			p->font=value.p;
			break;
	}
}

UI_PROPERTY_VALUE ui_label_get_prop(UI_WIDGET *widget, int prop) {
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	UI_PROPERTY_VALUE v={.p=NULL};
	switch(prop) {
		case UI_LABEL_PROP_FONT:
			v.p=p->font;
			break;
		case UI_LABEL_PROP_SURFACE:
			v.p=p->surface;
			break;
		case UI_LABEL_PROP_TEXT:
			v.p=p->text;
			break;
	}
	return v;
}

void ui_label_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	if(h==0||h==0)
		return;
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	if(p->surface!=NULL)
		darnitTextSurfaceFree(p->surface);
	int text_w;
	int text_h=darnitTextStringGeometrics(p->font, p->text, w, &text_w);
	p->surface=darnitTextSurfaceAlloc(p->font, darnitUtf8UnicodeCharactersInString(p->text), w, x+(w/2)-(text_w/2), y+(h/2)-(text_h/2));
	darnitTextSurfaceStringAppend(p->surface, p->text);
}

void ui_label_request_size(UI_WIDGET *widget, int *w, int *h) {
	int ww=w==NULL?32:*w;
	if((ww|*h)==0)
		return;
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	int text_w;
	int text_h=darnitTextStringGeometrics(p->font, p->text, ww, &text_w);
	*h=text_h;
}

void ui_label_render(UI_WIDGET *widget) {
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	darnitRenderBlendingEnable();
	darnitTextSurfaceDraw(p->surface);
	darnitRenderBlendingDisable();
}