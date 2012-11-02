#ifndef SPACER_H
#define SPACER_H

UI_WIDGET *ui_widget_create_spacer();

UI_PROPERTY_VALUE ui_spacer_get_prop(UI_WIDGET *widget, int prop);
void ui_spacer_set_prop(UI_WIDGET *widget, int prop, UI_PROPERTY_VALUE value);
void ui_spacer_request_size(UI_WIDGET *widget, int *w, int *h);
void ui_spacer_resize(UI_WIDGET *widget, int x, int y, int w, int h);
void ui_spacer_render(UI_WIDGET *widget);

#endif
