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

UI_PROPERTY_VALUE ui_imageview_get_prop(UI_WIDGET *widget, int prop);
void ui_imageview_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
void ui_imageview_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_imageview_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_imageview_render(UI_WIDGET *widget);

#endif
