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

#ifndef __MAP_PLATFORM_H__
#define	__MAP_PLATFORM_H__


void *ldmzLoad(const char *fname);
void *ldmzFree(void *map);
void ldmzGetSize(void *map, int *x, int *y);
unsigned int *ldmzGetData(void *map);
const char *ldmzFindProp(void *map, const char *key);
void *ldmzCache(const char *fname, unsigned int *len);

#endif
