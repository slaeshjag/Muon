#include <string.h>
#include "ui.h"

//UI pane

UI_PANE *ui_pane_create(int x, int y, int w, int h, UI_WIDGET *root_widget) {
	UI_PANE *pane;
	if((pane=malloc(sizeof(UI_PANE)))==NULL)
		return NULL;
	pane->x=x; pane->y=y;
	pane->w=w; pane->h=h;
	pane->background=darnitRenderRectAlloc(1);
	pane->border=darnitRenderLineAlloc(4, 2);
	pane->root_widget=NULL;
	ui_pane_resize(pane, x, y, w ,h);
	ui_pane_set_root_widget(pane, root_widget);
	return pane;
}

void ui_pane_destroy(UI_PANE *pane) {
	darnitRenderRectFree(pane->background);
	darnitRenderLineFree(pane->border);
	//if(pane->root_widget!=NULL)
		//free(pane->root_widget);
	free(pane);
}

void ui_pane_resize(UI_PANE *pane, int x, int y, int w, int h) {
	darnitRenderRectSet(pane->background, 0, x, y, x+w, y+h);
	darnitRenderLineMove(pane->border, 0, x, y, x+w, y);
	darnitRenderLineMove(pane->border, 1, x, y+h, x+w, y+h);
	darnitRenderLineMove(pane->border, 2, x, y, x, y+h);
	darnitRenderLineMove(pane->border, 3, x+w, y, x+w, y+h);
	if(pane->root_widget!=NULL)
		pane->root_widget->resize(pane->root_widget, x+UI_PADDING, y+UI_PADDING, w-UI_PADDING*2, h-UI_PADDING*2);
}

void ui_pane_set_root_widget(UI_PANE *pane, UI_WIDGET *root_widget) {
	pane->root_widget=root_widget;
	if(pane->root_widget!=NULL)
		pane->root_widget->resize(pane->root_widget, pane->x+UI_PADDING, pane->y+UI_PADDING, pane->w-UI_PADDING*2, pane->h-UI_PADDING*2);
}

void ui_pane_render(UI_PANE *pane) {
	float r, g, b, a;
	darnitRenderTintGet(&r, &g, &b, &a);
	darnitRenderTint(0, 0, 0, 1);
	darnitRenderRectDraw(pane->background, 1);
	darnitRenderTint(r, g, b, a);
	darnitRenderLineDraw(pane->border, 4);
	
	if(pane->root_widget!=NULL)
		pane->root_widget->render(pane->root_widget);
}

//UI label widget

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
	widget->set_prop=ui_label_set_prop;
	widget->get_prop=ui_label_get_prop;
	widget->resize=ui_label_resize;
	widget->render=ui_label_render;
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
void ui_label_render(UI_WIDGET *widget) {
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	darnitRenderBlendingEnable();
	darnitTextSurfaceDraw(p->surface);
	darnitRenderBlendingDisable();
}