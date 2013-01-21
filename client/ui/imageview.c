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
	p->border=d_render_line_new(4, 1);
	p->image_w=0;
	p->image_h=0;
	
	widget->destroy=ui_widget_destroy_imageview;
	widget->set_prop=ui_imageview_set_prop;
	widget->get_prop=ui_imageview_get_prop;
	widget->resize=ui_imageview_resize;
	widget->request_size=ui_imageview_request_size;
	widget->render=ui_imageview_render;
	widget->x=widget->y=widget->w=widget->h=0;
	widget->enabled=1;
	
	return widget;
}

void *ui_widget_destroy_imageview(UI_WIDGET *widget) {
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	d_render_tile_free(p->tile);
	d_render_tilesheet_free(p->tilesheet);
	d_render_line_free(p->border);
	return ui_widget_destroy(widget);
}

UI_WIDGET *ui_widget_create_imageview_raw(int w, int h, int pixel_format) {
	UI_WIDGET *widget;
	widget=ui_widget_create_imageview();
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	int tw, th;
	for(tw=1; w>=tw; tw<<=1);
	for(th=1; h>=th; th<<=1);
	p->tilesheet=d_render_tilesheet_new(1, 1, tw, th, pixel_format);
	p->tile=d_render_tile_new(1, p->tilesheet);
	p->image_w=w;
	p->image_h=h;
	return widget;
}

UI_WIDGET *ui_widget_create_imageview_file(const char *filename, int w, int h, int pixel_format) {
	UI_WIDGET *widget;
	widget=ui_widget_create_imageview();
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	p->tilesheet=d_render_tilesheet_load(filename, w, h, pixel_format);
	p->tile=d_render_tile_new(1, p->tilesheet);
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
	
	d_render_line_move(p->border, 0, x, y, x+w, y);
	d_render_line_move(p->border, 1, x, y+h, x+w, y+h);
	d_render_line_move(p->border, 2, x, y, x, y+h);
	d_render_line_move(p->border, 3, x+w, y, x+w, y+h);
	
	d_render_tile_move(p->tile, 0, x+w/2-p->image_w/2, y+h/2-p->image_h/2);
	d_render_tile_tilesheet_coord_set(p->tile, 0, 0, 0, p->image_w, p->image_h);
}

void ui_imageview_render(UI_WIDGET *widget) {
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(255, 255, 255, 255);
	d_render_tile_draw(p->tile, 1);
	d_render_tint(r, g, b, a);
	d_render_line_draw(p->border, 4);
}
