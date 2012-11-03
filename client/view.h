#ifndef VIEW_H
#define VIEW_H

#include "ui/ui.h"

DARNIT_FONT *font_std;
DARNIT_TILESHEET *mouse_tilesheet;

void view_init();
void view_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
