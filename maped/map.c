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

#include "maped.h"
#include "map.h"

DARNIT_MAP *map_new(unsigned int width, unsigned int height, unsigned int terrain_layers, DARNIT_TILESHEET *ts) {
	int i, j;
	int tile_w, tile_h;
	int fail=0;
	DARNIT_MAP *map=NULL;
	DARNIT_MAP_LAYER *layer=NULL;
	
	if(!(ts&&terrain_layers&&(map=malloc(sizeof(DARNIT_MAP)))&&(layer=malloc((terrain_layers+2)*sizeof(DARNIT_MAP_LAYER))))) {
		free(map);
		free(layer);
		return NULL;
	}
	
	map->layer=layer;
	*((unsigned int *) &map->layers)=terrain_layers+2;
	map->object=NULL;
	*((unsigned int *) &map->objects)=0;
	map->prop=NULL;
	map->stringdata=NULL;
	map->stringrefs=NULL;
	
	d_render_tilesheet_geometrics(ts, NULL, NULL, &tile_w, &tile_h);
	for(i=0; i<terrain_layers+2; i++) {
		/*This is needed to at least null all tilemaps so we can free them i one fails*/
		if(!(fail|=!(layer[i].tilemap=d_tilemap_new(0xFFF, ts, 0xFFF, width, height)))) {
			for(j=0; j<width*height; j++)
				layer[i].tilemap->data[j]=144*(!i);
			d_tilemap_recalc(layer[i].tilemap);
		}
		layer[i].ts=ts;
		layer[i].offset_x=0;
		layer[i].offset_y=0;
		layer[i].tile_w=tile_w;
		layer[i].tile_h=tile_h;
	}
	if(fail) {
		for(i=0; i<terrain_layers+2; i++)
			d_tilemap_free(layer[i].tilemap);
		free(map);
		free(layer);
		return NULL;
	}
	
	d_map_camera_move(map, 0, 0);
		
	return map;
}
