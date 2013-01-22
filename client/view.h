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

#ifndef VIEW_H
#define VIEW_H

#include "ui/ui.h"

DARNIT_FONT *font_std;
DARNIT_TILESHEET *mouse_tilesheet;
DARNIT_TILESHEET *mouse_target_tilesheet;

int view_background_w, view_background_h;
DARNIT_TILESHEET *view_background_ts;
DARNIT_TILE *view_background_tile;
unsigned int *view_background_pixbuf;

void view_init();
void view_background_update(int t);
void view_background_draw();
void view_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
