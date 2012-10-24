#ifndef VIEW_H
#define VIEW_H

#include "muon.h"

#define SIDEBAR_WIDTH 128

void view_move(DARNIT_MOUSE mouse);
void view_draw();

DARNIT_FONT *font_std;

DARNIT_RECT *sidebar;
DARNIT_LINE *sidebar_lines;
DARNIT_TEXT_SURFACE *sidebar_text_title;
DARNIT_TEXT_SURFACE *sidebar_text_buildings;

#endif