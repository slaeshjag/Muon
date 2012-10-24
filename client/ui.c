#include "ui.h"

//UI pane

UI_PANE *ui_pane_create(int x, int y, int w, int h, UI_WIDGET *root_widget) {
	UI_PANE *pane;
	if((pane=malloc(sizeof(UI_PANE)))==NULL)
		return NULL;
	pane->x=x; pane->y=y;
	pane->w=w; pane->h=h;
	pane->border=darnitRenderLineAlloc(4, 2);
	ui_pane_resize(pane, x, y, w ,h);
	ui_pane_set_root_widget(pane, root_widget);
}

void ui_pane_destroy(UI_PANE *pane) {
	darnitRenderLineFree(pane->border);
	//if(pane->root_widget!=NULL)
		//free(pane->root_widget);
	free(pane);
}

void ui_pane_resize(UI_PANE *pane, int x, int y, int w, int h) {
	darnitRenderLineMove(pane->border, 0, x, y, x+w, y);
	darnitRenderLineMove(pane->border, 1, x, y+h, x+w, y+h);
	darnitRenderLineMove(pane->border, 2, x, y, x, y+h);
	darnitRenderLineMove(pane->border, 3, x+w, y, x+w, y+h);
}

void ui_pane_set_root_widget(UI_PANE *pane, UI_WIDGET *root_widget) {
	pane->root_widget=root_widget;
}

void ui_pane_render(UI_PANE *pane) {
	darnitRenderLineDraw(pane->border, 4);
	
	if(pane->root_widget!=NULL)
		pane->root_widget->render(pane->root_widget, pane->x+2, pane->y+4, pane->w-8, pane->h-8);
}

//UI label widget

UI_WIDGET *ui_widget_create_label(DARNIT_FONT *font, char *text) {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	widget->set_prop=ui_label_set_prop;
	widget->get_prop=ui_label_get_prop;
	UI_PROPERTY_VALUE v={.p=font};
	widget->set_prop(widget, UI_LABEL_PROP_FONT, v);
	v.p=text;
	widget->set_prop(widget, UI_LABEL_PROP_TEXT, v);
}
void ui_label_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_LABEL_PROP_TEXT:
			p->text=value.p;
			break;
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
		case UI_LABEL_PROP_TEXT:
			v.p=p->text;
			break;
	}
	return v;
}
void ui_label_render(UI_WIDGET *widget, int x, int y, int w, int h) {
	
}