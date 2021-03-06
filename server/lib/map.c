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

#include "server.h"
#include <darnit/darnit.h>

void *ldmzLoad(const char *fname) {
	void *ret;
	d_fs_mount(fname);
	ret = d_map_load("mapdata/map.ldmz");
	d_fs_unmount(fname);

	return ret;
}


void ldmzGetSize(void *map, int *x, int *y) {
	DARNIT_MAP *dmap = map;
	*x = dmap->layer->tilemap->w;
	*y = dmap->layer->tilemap->h;

	return;
}


unsigned int *ldmzGetData(void *map) {
	DARNIT_MAP *dmap = map;
	
	return dmap->layer[dmap->layers-2].tilemap->data;
}


void *ldmzFree(void *map) {
	return d_map_unload(map);
}


const char *ldmzFindProp(void *map, const char *key) {
	DARNIT_MAP *dmap = map;

	return d_map_prop(dmap->prop, key);
}


void *ldmzCache(const char *fname, unsigned int *len) {
	DARNIT_FILE *f;
	void *data;

	if (!(f = d_file_open(fname, "rb")))
		return NULL;
	
	d_file_seek(f, 0, SEEK_END);
	*len = d_file_tell(f);
	if (!(data = malloc(d_file_tell(f)))) {
		d_file_close(f);
		return NULL;
	}

	d_file_seek(f, 0, SEEK_SET);
	d_file_read(data, *len, f);
		
	d_file_close(f);

	return data;
}
