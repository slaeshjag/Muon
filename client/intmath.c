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

#include <math.h>

#include "intmath.h"

void intmath_init() {
	int i;

	for (i = 0; i < 256; i++)
		sinetable[i] = sin(M_PI*i/128)*256*8;
}

int sine(int angle) {
	//angle%=360;
	//angle&=0xFF;
	if (angle<0)
		angle=~angle+1;
	return sinetable[angle&0xFF];
}
