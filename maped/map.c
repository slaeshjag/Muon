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
#include <zlib.h>

#include "maped.h"
#include "map.h"

MAP *map_new(unsigned int width, unsigned int height, unsigned int terrain_layers, DARNIT_TILESHEET *ts) {
	int i, j;
	int tile_w, tile_h;
	int fail=0;
	DARNIT_MAP *d_map=NULL;
	DARNIT_MAP_LAYER *layer=NULL;
	MAP *map;
	map=malloc(sizeof(MAP));
	
	if(!(ts&&terrain_layers&&(d_map=malloc(sizeof(DARNIT_MAP)))&&(layer=malloc((terrain_layers+2)*sizeof(DARNIT_MAP_LAYER))))) {
		free(d_map);
		free(layer);
		return NULL;
	}
	
	d_map->layer=layer;
	*((unsigned int *) &d_map->layers)=terrain_layers+2;
	d_map->object=NULL;
	*((unsigned int *) &d_map->objects)=0;
	d_map->prop=NULL;
	d_map->stringdata=NULL;
	d_map->stringrefs=NULL;
	
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
		free(d_map);
		free(layer);
		return NULL;
	}
	
	d_map_camera_move(d_map, 0, 0);
	
	map->map=d_map;
	map->stringtable=malloc(sizeof(MAP_PROPERTY));
	map->stringtable->key="tileset";
	map->stringtable->value="default.png";
	map->stringtable->next=NULL;
	map->tilesheet=NULL;
	map->tilesheet_size=0;
	map_save(map, "testmap.ldmz");
	return map;
}

void map_save(MAP *map, const char *filename) {
	int i, j;
	LDI_MAIN m;
	FILE *f;
	struct LDMZ ldmz;
	MAP_PROPERTY *stringtable, **st_add;
	char *stringtable_data, *p;
	int32_t *ref_data;
	unsigned long zsize;
	uint32_t (*layer_header)[8];
	uint32_t header[12];
	unsigned int stringtable_layerref;
	
	header[0]=ldmz.header.magic=LDMZ_MAGIC;
	header[1]=ldmz.header.version=LDMZ_VERSION;
	header[6]=ldmz.header.layers=map->map->layers;
	ldmz.layer=malloc(ldmz.header.layers*sizeof(struct LDMZ_LAYER));
	header[8]=ldmz.header.objects=0; /*map->map->objects; nope.jpg*/
	header[9]=ldmz.header.objects_zsize=0;
	ldmz.object=NULL;
	ldmz.objectz=NULL;
	
	
	ldmz.header.stringtable_size=0;
	for(st_add=&(map->stringtable), i=0; *st_add; st_add=&((*st_add)->next), i++)
		ldmz.header.stringtable_size+=strlen((*st_add)->key)+strlen((*st_add)->value)+2;
	*st_add=malloc(sizeof(MAP_PROPERTY));
	(*st_add)->key=(*st_add)->value=(void *) ((*st_add)->next=NULL);
	for(st_add=&((*st_add)->next), j=0; j<ldmz.header.layers*2; st_add=&((*st_add)->next), j++ ) {
		*st_add=malloc(sizeof(MAP_PROPERTY));
		(*st_add)->next=NULL;
		if(j&1) {
			(*st_add)->key=(*st_add)->value=NULL;
		} else {
			(*st_add)->key="NAME";
			(*st_add)->value="lol";
			ldmz.header.stringtable_size+=strlen((*st_add)->key)+strlen((*st_add)->value)+2;
		}
	}
	
	stringtable_data=malloc(header[2]=ldmz.header.stringtable_size);
	stringtable_layerref=i+1;
	header[10]=ldmz.header.refs=i+1+ldmz.header.layers*2;
	header[4]=ldmz.header.refs_size=sizeof(int32_t)*ldmz.header.refs*2;
	header[11]=ldmz.header.main_ref_index=0;
	ref_data=malloc(ldmz.header.refs_size);
	for(stringtable=map->stringtable, i=0, p=stringtable_data; stringtable; stringtable=stringtable->next) {
		unsigned int size;
		if(!stringtable->key) {
			ref_data[i++]=-1;
			ref_data[i++]=-1;
			continue;
		}
		strcpy(p, stringtable->key);
		size=strlen(stringtable->key)+1;
		ref_data[i++]=d_util_htonl(p-stringtable_data);
		p+=size;
		strcpy(p, stringtable->value);
		size=strlen(stringtable->value)+1;
		ref_data[i++]=d_util_htonl(p-stringtable_data);
		p+=size;
	}
	
	
	//TODO: add layer names to stringdata!
	ldmz.stringtablez=malloc(zsize=compressBound(ldmz.header.stringtable_size));
	compress(ldmz.stringtablez, &zsize, (void *) stringtable_data, ldmz.header.stringtable_size);
	header[3]=ldmz.header.stringtable_zsize=zsize;
	free(stringtable_data);
	ldmz.refsz=malloc(zsize=compressBound(ldmz.header.refs_size));
	compress(ldmz.refsz, &zsize, (void *) ref_data, ldmz.header.refs_size);
	header[5]=ldmz.header.refs_zsize=zsize;
	free(ref_data);
	
	layer_header=malloc(sizeof(uint32_t)*8*ldmz.header.layers);
	for(i=0; i<map->map->layers; i++) {
		unsigned int size;
		unsigned int *data;
		layer_header[i][0]=d_util_htonl(ldmz.layer[i].width=map->map->layer[i].tilemap->w);
		layer_header[i][1]=d_util_htonl(ldmz.layer[i].height=map->map->layer[i].tilemap->h);
		layer_header[i][2]=d_util_htonl(ldmz.layer[i].offset_x=map->map->layer[i].offset_x);
		layer_header[i][3]=d_util_htonl(ldmz.layer[i].offset_y=map->map->layer[i].offset_y);
		
		layer_header[i][5]=d_util_htonl(ldmz.layer[i].ref_index=stringtable_layerref+i*2);
		layer_header[i][6]=d_util_htonl(ldmz.layer[i].tile_w=map->map->layer[i].tile_w);
		layer_header[i][7]=d_util_htonl(ldmz.layer[i].tile_h=map->map->layer[i].tile_h);
		
		size=sizeof(uint32_t)*map->map->layer[i].tilemap->w*map->map->layer[i].tilemap->h;
		ldmz.layer[i].dataz=malloc(zsize=compressBound(size));
		data=malloc(size);
		memcpy(data, map->map->layer[i].tilemap->data, size);
		d_util_endian_convert(data, size/sizeof(uint32_t));
		compress((void *) ldmz.layer[i].dataz, &zsize, (void *) data, size);
		free(data);
		layer_header[i][4]=d_util_htonl(ldmz.layer[i].zsize=((unsigned int) zsize));
	}
	ldmz.layer_headerz=malloc(zsize=compressBound(sizeof(uint32_t)*8*ldmz.header.layers));
	compress((void *) ldmz.layer_headerz, &zsize, (void *) layer_header, sizeof(uint32_t)*8*ldmz.header.layers);
	header[7]=ldmz.header.layer_headers_zsize=zsize;
	
	if(!(f=fopen(filename, "wb")))
		return;
	
	d_util_endian_convert(header, 12);
	fwrite(&header, sizeof(uint32_t), 12, f);
	fwrite(ldmz.stringtablez, ldmz.header.stringtable_zsize, 1, f);
	fwrite(ldmz.refsz, ldmz.header.refs_zsize, 1, f);
	fwrite(ldmz.layer_headerz, ldmz.header.layer_headers_zsize, 1, f);
	for(i=0; i<ldmz.header.layers; i++)
		fwrite(ldmz.layer[i].dataz, ldmz.layer[i].zsize, 1, f);
		
	/*
	m.magic=d_util_htonl(LDI_MAGIC);
	m.version=d_util_htonl(LDI_VERSION);
	m.files=d_util_htonl(2);
	fwrite(&m.magic, 4, 1, f);
	fwrite(&m.version, 4, 1, f);
	fwrite(&m.files, 4, 1, f);*/
	
	fclose(f);
}
