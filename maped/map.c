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
#include <string.h>

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

void map_save(MAP *map, const char *filename) {
	int i;
	LDI_MAIN m;
	FILE *f;
	struct LDMZ ldmz;
	struct MAP_PROPERTY *stringtable;
	char *stringtable_data, *p;
	unsigned char *stringtable_zdata;
	unsigned int stringtable_nullref;
	int32_t *ref_data;
	unsigned char *ref_zdata;
	
	//OOPS: must convert endianess..
	ldmz.header.magic=LDMZ_MAGIC;
	ldmz.header.version=LDMZ_VERSION;
	ldmz.header.layers=map->map->layers;
	ldmz.layer=malloc(ldmz.header.layers*sizeof(struct LDMZ_LAYER));
	ldmz.header.objects=0; /*map->map->objects; nope.jpg*/
	ldmz.object=NULL;
	ldmz.objectz=NULL;
	
	ldmz.header.stringtable_size=0;
	/*Yeah two passes are inefficient, but wth*/
	for(stringtable=map->stringtable, i=0; stringtable; stringtable=stringtable->next, i++)
		ldmz.header.stringtable_size+=strlen(stringtable->key)+strlen(stringtable->value)+2;
	stringtable_data=malloc(ldmz.header.stringtable_size);
	ldmz.header.ref_size=sizeof(int32_t)*(i+1)*2;
	ref_data=malloc(ldmz.header.ref_size);
	stringtable_nullref=ldmz.header.ref_size-2;
	ref_data[stringtable_nullref]=ref_data[stringtable_nullref+1]=-1;
	for(stringtable=map->stringtable, i=0, p=stringtable_data; stringtable; stringtable=stringtable->next) {
		unsigned int size;
		strcpy(p, stringtable->key);
		size=strlen(stringtable->key)+1;
		ref_data[i++]=d_util_htonl(p-stringtable_data);
		p+=size;
		strcpy(p, stringtable->value);
		size=strlen(stringtable->value)+1;
		ref_data[i++]=d_util_htonl(p-stringtable_data);
		p+=size;
	}
	//compress stringdata and refs, free stuff
	free(stringtable_data);
	free(ref_data);
	
	for(i=0; i<map->map->layers; i++) {
		ldmz.layer[i].width=d_util_htonl(map->map->layer[i].tilemap->w);
		ldmz.layer[i].height=d_util_htonl(map->map->layer[i].tilemap->h);
		ldmz.layer[i].offset_x=d_util_htonl(map->map->layer[i].offset_x);
		ldmz.layer[i].offset_y=d_util_htonl(map->map->layer[i].offset_y);
		ldmz.layer[i].tile_w=d_util_htonl(map->map->layer[i].tile_w);
		ldmz.layer[i].tile_h=d_util_htonl(map->map->layer[i].tile_h);
		ldmz.layer[i].ref_index=d_util_htonl(stringtable_nullref);
		//compress layer data
	}
	//compress layer header
	
	if(!(f=fopen(filename, "wb")))
		return;
	m.magic=d_util_htonl(LDI_MAGIC);
	m.version=d_util_htonl(LDI_VERSION);
	m.files=d_util_htonl(2);
	fwrite(&m.magic, 4, 1, f);
	fwrite(&m.version, 4, 1, f);
	fwrite(&m.files, 4, 1, f);
	
	fclose(f);
}
