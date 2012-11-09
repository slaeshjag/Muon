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

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#define UI_IMAGEVIEW_PROP_TILESHEET 0
#define UI_IMAGEVIEW_PROP_TILE 1
#define UI_IMAGEVIEW_PROP_BORDER 2
#define UI_IMAGEVIEW_PROP_IMAGE_WIDTH 3
#define UI_IMAGEVIEW_PROP_IMAGE_HEIGHT 4

struct UI_IMAGEVIEW_PROPERTIES {
	DARNIT_TILESHEET *tilesheet;
	DARNIT_TILE *tile;
	DARNIT_LINE *border;
	int image_w;
	int image_h;
};

UI_WIDGET *ui_widget_create_imageview();
UI_WIDGET *ui_widget_create_imageview_raw(int w, int h, int pixel_format);
UI_WIDGET *ui_widget_create_imageview_file(const char *filename, int w, int h, int pixel_format);
void *ui_widget_destroy_imageview(UI_WIDGET *widget);

UI_PROPERTY_VALUE ui_imageview_get_prop(UI_WIDGET *widget, int prop);
void ui_imageview_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
void ui_imageview_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_imageview_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_imageview_render(UI_WIDGET *widget);

#endif
