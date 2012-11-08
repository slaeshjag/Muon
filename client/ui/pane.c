#include "ui.h"

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

void *ui_pane_destroy(UI_PANE *pane) {
	darnitRenderLineFree(pane->border);
	darnitRenderRectFree(pane->background);
	free(pane);
	return NULL;
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
