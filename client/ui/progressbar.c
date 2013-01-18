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

#include <stdio.h>
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
	p->border=d_render_line_new(4, 1);
	p->bar=d_render_rect_new(1);
	p->progress=0;
	strcpy(p->text, "0%");
	
	widget->destroy=ui_widget_destroy_progressbar;
	widget->set_prop=ui_progressbar_set_prop;
	widget->get_prop=ui_progressbar_get_prop;
	widget->resize=ui_progressbar_resize;
	widget->request_size=ui_progressbar_request_size;
	widget->render=ui_progressbar_render;
	widget->x=widget->y=widget->w=widget->h=0;
	widget->enabled=1;

	return widget;
}

void *ui_widget_destroy_progressbar(UI_WIDGET *widget) {
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	d_render_rect_free(p->bar);
	d_render_line_free(p->border);
	d_text_surface_free(p->surface);
	return ui_widget_destroy(widget);
}

void ui_progressbar_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value) {
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_PROGRESSBAR_PROP_PROGRESS:
			if(value.i<0||value.i>100)
				break;
			p->progress=value.i;
			sprintf(p->text, "%i%%", value.i);
			d_render_rect_move(p->bar, 0, widget->x+2, widget->y+2, widget->x+2+((widget->w-4)*value.i/100), widget->y+widget->h-2);
			if(p->surface!=NULL)
				d_text_surface_free(p->surface);
			int text_w;
			int text_h=d_font_string_geometrics(p->font, p->text, widget->w, &text_w);
			p->surface=d_text_surface_new(p->font, d_utf8_chars_in_string(p->text), widget->w, widget->x+(widget->w/2)-(text_w/2), widget->y+(widget->h/2)-(text_h/2));
			d_text_surface_string_append(p->surface, p->text);
			break;
	}
}

UI_PROPERTY_VALUE ui_progressbar_get_prop(UI_WIDGET *widget, int prop) {
	UI_PROPERTY_VALUE v={.p=NULL};
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	switch(prop) {
		case UI_PROGRESSBAR_PROP_PROGRESS:
			v.i=p->progress;
			break;
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
	
	d_render_line_move(p->border, 0, x, y, x+w, y);
	d_render_line_move(p->border, 1, x, y+h, x+w, y+h);
	d_render_line_move(p->border, 2, x, y, x, y+h);
	d_render_line_move(p->border, 3, x+w, y, x+w, y+h);
	
	//d_render_rect_move(p->bar, 0, widget->x+2, widget->y+2, widget->x+widget->w-2, widget->y+widget->h-2);
	d_render_rect_move(p->bar, 0, widget->x+2, widget->y+2, widget->x+2+((widget->w-4)*p->progress/100), widget->y+widget->h-2);
	
	if(p->surface!=NULL)
		d_text_surface_free(p->surface);
	int text_w;
	int text_h=d_font_string_geometrics(p->font, p->text, w, &text_w);
	p->surface=d_text_surface_new(p->font, d_utf8_chars_in_string(p->text), w, x+(w/2)-(text_w/2), y+(h/2)-(text_h/2));
	d_text_surface_string_append(p->surface, p->text);
}

void ui_progressbar_request_size(UI_WIDGET *widget, int *w, int *h) {
	int ww=64+4;
	if(w)
		*w=ww;
	if(!h)
		return;
	struct UI_LABEL_PROPERTIES *p=widget->properties;
	int text_h=d_font_glyph_h(p->font);
	//d_font_string_geometrics(p->font, p->offset, ww, &text_w);
	*h=text_h+4;
}

void ui_progressbar_render(UI_WIDGET *widget) {
	struct UI_PROGRESSBAR_PROPERTIES *p=widget->properties;
	d_render_line_draw(p->border, 4);
	
	d_render_blend_enable();
	d_text_surface_draw(p->surface);
	d_render_blend_disable();
	d_render_logic_op(DARNIT_RENDER_LOGIC_OP_XOR);
	d_render_rect_draw(p->bar, 1);
	d_render_logic_op(DARNIT_RENDER_LOGIC_OP_NONE);
}
