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

#include "../intmath.h"
#include "../view.h"
#include "../platform.h"
#include "credits.h"

void credits_background_update(int t) {
	register int x, y, mov1, mov2, c1, c2, c3, c;
	for (y=0; y<view_background_h; y++)
		for (x=0; x<view_background_w; x++) {
			mov1=(y<<8)/view_background_h+(t>>1);
			mov2=(x<<8)/view_background_w;
			c1=sine(mov1+(t>>1))/2+((mov2>>1)-mov1-mov2+(t>>1));
			c2=sine((c1+sine((y>>2)+(t>>1))+sine((/*x+*/100+y)))/8);
			c3=sine((c2+(cosine(mov1+mov2+c2/8)>>2)+cosine(mov2)+sine(x))/8);
			c=(c1+c2+c3)/256+32;
			view_background_pixbuf[y*view_background_w+x]=c|c<<8|c<<16;
		}
	d_render_tilesheet_update(view_background_ts, 0, 0, view_background_w, view_background_h, view_background_pixbuf);
}

void credits_background_draw() {
	static int t=0, recalc=1;
	if(config.plasma&&recalc) {
		credits_background_update(t);
		t++;
	}
	recalc=!recalc;
	
	d_render_tile_draw(view_background_tile, 1);
}

void credits_draw() {
	static int fade=0, tint=-127;
	//printf("arne %i\n", d_render_fade_status());
	if(d_render_fade_status()==1) {
		view_background_draw();
		fade=1;
		return;
	}
	if(fade) {
		d_render_fade_out(500);
		fade=0;
	}
	d_render_tint(0, 0, 127+ABS(tint)%128, 255);
	credits_background_draw();
	d_render_tint(255, 255, 255, 255);
	if(++tint==128)
		tint=-127;
}
