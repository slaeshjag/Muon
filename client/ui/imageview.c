#include "ui.h"

UI_WIDGET *ui_widget_create_imageview() {
	UI_WIDGET *widget;
	if((widget=malloc(sizeof(UI_WIDGET)))==NULL)
		return NULL;
	if((widget->properties=malloc(sizeof(struct UI_IMAGEVIEW_PROPERTIES)))==NULL) {
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
	
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	p->tile=NULL;
	p->tilesheet=NULL;
	p->border=darnitRenderLineAlloc(4, 1);
	p->image_w=0;
	p->image_h=0;
	
	widget->destroy=ui_widget_destroy_imageview;
	widget->set_prop=ui_imageview_set_prop;
	widget->get_prop=ui_imageview_get_prop;
	widget->resize=ui_imageview_resize;
	widget->request_size=ui_imageview_request_size;
	widget->render=ui_imageview_render;
	widget->x=widget->y=widget->w=widget->h=0;
	
	return widget;
}

void *ui_widget_destroy_imageview(UI_WIDGET *widget) {
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	darnitRenderTileFree(p->tile);
	darnitRenderTilesheetFree(p->tilesheet);
	darnitRenderLineFree(p->border);
	return ui_widget_destroy(widget);
}

UI_WIDGET *ui_widget_create_imageview_raw(int w, int h, int pixel_format) {
	UI_WIDGET *widget;
	widget=ui_widget_create_imageview();
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	int tw, th;
	for(tw=1; w>=tw; tw<<=1);
	for(th=1; h>=th; th<<=1);
	p->tilesheet=darnitRenderTilesheetNew(1, 1, tw, th, pixel_format);
	p->tile=darnitRenderTileAlloc(1);
	p->image_w=w;
	p->image_h=h;
	return widget;
}

UI_WIDGET *ui_widget_create_imageview_file(const char *filename, int w, int h, int pixel_format) {
	UI_WIDGET *widget;
	widget=ui_widget_create_imageview();
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	int tw, th;
	for(tw=1; w<=tw; tw<<=1);
	for(th=1; h<=th; th<<=1);
	p->tilesheet=darnitRenderTilesheetLoad(filename, w, h, pixel_format);
	p->tile=darnitRenderTileAlloc(1);
	p->image_w=w;
	p->image_h=h;
	return widget;
}

UI_PROPERTY_VALUE ui_imageview_get_prop(UI_WIDGET *widget, int prop) {
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	UI_PROPERTY_VALUE v={.p=NULL};
	switch(prop) {
		case UI_IMAGEVIEW_PROP_TILESHEET:
			v.p=p->tilesheet;
			break;
		case UI_IMAGEVIEW_PROP_TILE:
			v.p=p->tile;
			break;
		case UI_IMAGEVIEW_PROP_IMAGE_WIDTH:
			v.i=p->image_w;
			break;
		case UI_IMAGEVIEW_PROP_IMAGE_HEIGHT:
			v.i=p->image_h;
			break;
	}
	return v;
}

void ui_imageview_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	return;
}

void ui_imageview_request_size(UI_WIDGET *widget, int *w, int *h) {
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	if(w)
		*w=p->image_w;
	if(h)
		*h=p->image_h;
}

void ui_imageview_resize(UI_WIDGET *widget, int x, int y, int w, int h) {
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	widget->x=x; widget->y=y;
	widget->w=w; widget->h=h;
	
	darnitRenderLineMove(p->border, 0, x, y, x+w, y);
	darnitRenderLineMove(p->border, 1, x, y+h, x+w, y+h);
	darnitRenderLineMove(p->border, 2, x, y, x, y+h);
	darnitRenderLineMove(p->border, 3, x+w, y, x+w, y+h);
	
	darnitRenderTileMove(p->tile, 0, p->tilesheet, x, y);
	darnitRenderTileSetTilesheetCoord(p->tile, 0, p->tilesheet, 0, 0, w, h);
}

void ui_imageview_render(UI_WIDGET *widget) {
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	float r, g, b, a;
	darnitRenderTintGet(&r, &g, &b, &a);
	darnitRenderTint(1, 1, 1, 1);
	darnitRenderTileDraw(p->tile, p->tilesheet, 1);
	darnitRenderTint(r, g, b, a);
	darnitRenderLineDraw(p->border, 4);
}
