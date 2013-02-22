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
	FILE *f_ts;
	if(!(map=malloc(sizeof(MAP))))
		return NULL;
	
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
				layer[i].tilemap->data[j]=i==terrain_layers+1?1:144*(!i);
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
	map->stringtable->value="mapdata/default.png";
	map->stringtable->next=NULL;
	
	map->w=width*tile_w;
	map->h=height*tile_h;
	sprintf(map->sizestring, "%ix%i", width, height);
	map_prop_set_or_add(map, "size", map->sizestring);
	
	f_ts=fopen("res/default.png", "rb");
	fseek(f_ts, 0, SEEK_END);
	map->tilesheet_size=ftell(f_ts);
	map->tilesheet=malloc(map->tilesheet_size);
	fseek(f_ts, 0, SEEK_SET);
	fread(map->tilesheet, map->tilesheet_size, 1, f_ts);
	fclose(f_ts);
	
	return map;
}

DARNIT_MAP *map_new_palette(unsigned int width, unsigned int height, DARNIT_TILESHEET *ts) {
	int i;
	int tile_w, tile_h;
	DARNIT_MAP *d_map=NULL;
	DARNIT_MAP_LAYER *layer=NULL;
	
	if(!(ts&&(d_map=malloc(sizeof(DARNIT_MAP)))&&(layer=malloc(sizeof(DARNIT_MAP_LAYER))))) {
		free(d_map);
		free(layer);
		return NULL;
	}
	
	d_map->layer=layer;
	*((unsigned int *) &d_map->layers)=1;
	d_map->object=NULL;
	*((unsigned int *) &d_map->objects)=0;
	d_map->prop=NULL;
	d_map->stringdata=NULL;
	d_map->stringrefs=NULL;
	
	d_render_tilesheet_geometrics(ts, NULL, NULL, &tile_w, &tile_h);
	
		/*This is needed to at least null all tilemaps so we can free them i one fails*/
	if(!(layer->tilemap=d_tilemap_new(0xFFF, ts, 0xFFF, width, height))) {
		free(d_map);
		free(layer);
		return NULL;
	}
	for(i=0; i<width*height; i++)
		layer->tilemap->data[i]=0;
	d_tilemap_recalc(layer->tilemap);
		
	layer->ts=ts;
	layer->offset_x=0;
	layer->offset_y=0;
	layer->tile_w=tile_w;
	layer->tile_h=tile_h;
	
	d_map_camera_move(d_map, 0, 0);
	return d_map;
}

MAP *map_load(const char *filename) {
	MAP *map;
	DARNIT_FILE *f_ts;
	if(d_fs_mount(filename))
		return NULL;
	if(!(map=malloc(sizeof(MAP)))) {
		d_fs_unmount(filename);
		return NULL;
	}
	if(!(map->stringtable=malloc(sizeof(MAP_PROPERTY)))) {
		free(map);
		d_fs_unmount(filename);
		return NULL;
	}
	if(!(map->map=d_map_load("mapdata/map.ldmz"))) {
		free(map->stringtable);
		free(map);
		d_fs_unmount(filename);
		return NULL;
	}
	if(!(f_ts=d_file_open(d_map_prop(map->map->prop, "tileset"), "rb")))
		if(!(f_ts=fopen("res/default.png", "rb"))) {
			free(map->stringtable);
			free(map);
			d_fs_unmount(filename);
			return NULL;
		}
	
	map->stringtable->key="tileset";
	map->stringtable->value="mapdata/default.png";
	map->stringtable->next=NULL;
	
	d_file_seek(f_ts, 0, SEEK_END);
	map->tilesheet_size=d_file_tell(f_ts);
	map->tilesheet=malloc(map->tilesheet_size);
	d_file_seek(f_ts, 0, SEEK_SET);
	d_file_read(map->tilesheet, map->tilesheet_size, f_ts);
	d_file_close(f_ts);
	d_fs_unmount(filename);
	map->w=map->map->layer->tile_w*map->map->layer->tilemap->w;
	map->h=map->map->layer->tile_h*map->map->layer->tilemap->h;
	sprintf(map->sizestring, "%ix%i", map->map->layer->tilemap->w, map->map->layer->tilemap->h);
	map_prop_set_or_add(map, "size", map->sizestring);
	
	map_prop_set_or_add(map, "name", d_map_prop(map->map->prop, "name"));
	map_prop_set_or_add(map, "version", d_map_prop(map->map->prop, "version"));
	map_prop_set_or_add(map, "author", d_map_prop(map->map->prop, "author"));
	map_prop_set_or_add(map, "max_players", d_map_prop(map->map->prop, "max_players"));
	return map;
}

MAP *map_close(MAP *map) {
	MAP_PROPERTY *p;
	d_map_unload(map->map);
	while(map->stringtable) {
		p=map->stringtable;
		map->stringtable=p->next;
		free(p);
	}
	free(map->tilesheet);
	free(map);
	return NULL;
}

void map_prop_set_or_add(MAP *map, const char *key, const char *value) {
	MAP_PROPERTY **p;
	if(!map)
		return;
	
	for(p=&map->stringtable; *p; p=&((*p)->next)) {
		if(!strcmp((*p)->key, key)) {
			(*p)->value=value;
			return;
		}
	}
	
	*p=malloc(sizeof(MAP_PROPERTY));
	(*p)->key=key;
	(*p)->value=value;
	(*p)->next=NULL;
}

const char *map_prop_get(MAP *map, const char *key) {
	MAP_PROPERTY *p;
	if(!map)
		return NULL;
	for(p=map->stringtable; p; p=p->next)
		if(!strcmp(p->key, key))
			return p->value;
	return NULL;
}

void map_save(MAP *map, const char *filename) {
	int i, j;
	LDI_MAIN m;
	LDI_ENTRY *entry;
	DARNIT_FILE *f;
	struct LDMZ ldmz;
	MAP_PROPERTY *stringtable, **st_add, **st_temp;
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
	/*So we can later clean out the added elements*/
	st_temp=st_add;
	*st_add=malloc(sizeof(MAP_PROPERTY));
	(*st_add)->key=(*st_add)->value=(void *) ((*st_add)->next=NULL);
	for(st_add=&((*st_add)->next), j=0; j<ldmz.header.layers*2; st_add=&((*st_add)->next), j++ ) {
		*st_add=malloc(sizeof(MAP_PROPERTY));
		(*st_add)->next=NULL;
		if(j&1) {
			(*st_add)->key=(*st_add)->value=NULL;
		} else {
			(*st_add)->key="NAME";
			(*st_add)->value="layer";
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
	free(layer_header);
	
	if(!(f=d_file_open(filename, "wb"))) {
		ui_messagebox(font_std, "Unable to save map");
		goto map_save_error;
	}
	
	
	m.magic=d_util_htonl(LDI_MAGIC);
	m.version=d_util_htonl(LDI_VERSION);
	m.files=d_util_htonl(2);
	d_file_write(&m.magic, sizeof(uint32_t), f);
	d_file_write(&m.version, sizeof(uint32_t), f);
	d_file_write(&m.files, sizeof(uint32_t), f);
	m.files=d_util_ntohl(m.files);
	
	entry=calloc(sizeof(LDI_ENTRY), m.files);
	strcpy(entry[0].name, "mapdata/map.ldmz");
	entry[0].len=sizeof(uint32_t)*12+ldmz.header.stringtable_zsize+ldmz.header.refs_zsize+ldmz.header.layer_headers_zsize;
	for(i=0; i<ldmz.header.layers; i++)
		entry[0].len+=ldmz.layer[i].zsize;
	entry[0].len=d_util_htonl(entry[0].len);
	strcpy(entry[1].name, "mapdata/default.png");
	entry[1].pos=entry[0].len;
	entry[1].len=d_util_htonl(map->tilesheet_size);
	
	for(i=0; i<m.files; i++) {
		d_file_write(entry[i].name, LDI_FILENAME_LEN, f);
		d_file_write(&entry[i].pos, sizeof(uint32_t), f);
		d_file_write(&entry[i].len, sizeof(uint32_t), f);
		d_file_write(&entry[i].pad, sizeof(uint32_t), f);
	}
	free(entry);
	
	d_util_endian_convert(header, 12);
	d_file_write(&header, sizeof(uint32_t)*12, f);
	d_file_write(ldmz.stringtablez, ldmz.header.stringtable_zsize, f);
	d_file_write(ldmz.refsz, ldmz.header.refs_zsize, f);
	d_file_write(ldmz.layer_headerz, ldmz.header.layer_headers_zsize, f);
	for(i=0; i<ldmz.header.layers; i++)
		d_file_write(ldmz.layer[i].dataz, ldmz.layer[i].zsize, f);
	
	d_file_write(map->tilesheet, map->tilesheet_size, f);
	
	map_save_error:
	d_file_close(f);
	free(ldmz.stringtablez);
	free(ldmz.refsz);
	free(ldmz.layer_headerz);
	for(i=0; i<ldmz.header.layers; i++)
		free(ldmz.layer[i].dataz);
	free(ldmz.layer);
	while(*st_temp) {
		stringtable=(*st_temp)->next;
		free(*st_temp);
		*st_temp=stringtable;
	}
	
}

void map_tile_set(DARNIT_TILEMAP *tilemap, int x, int y, int mirrorx, int mirrory, unsigned int tile) {
	int i;
	if(x<0||y<0||x>=tilemap->w||y>=tilemap->h)
		return;
	tilemap->data[y*tilemap->w+x]=tile;
	
	map->map->layer[map->map->layers-2].tilemap->data[y*map->map->layer[map->map->layers-2].tilemap->w+x]|=(1<<17);
	for(i=0; i<map->map->layers-2; i++)
		if(map->map->layer[i].tilemap->data[y*map->map->layer[i].tilemap->w+x]) {
			map->map->layer[map->map->layers-2].tilemap->data[y*map->map->layer[map->map->layers-2].tilemap->w+x]&=~(1<<17);
			break;
		}
	
	if(mirrorx)
		map_tile_set(tilemap, abs(tilemap->w-1-x), y, 0, 0, tile);
	if(mirrory)
		map_tile_set(tilemap, x, abs(tilemap->h-1-y), 0, 0, tile);
	if(mirrorx&&mirrory)
		map_tile_set(tilemap, abs(tilemap->w-1-x), abs(tilemap->h-1-y), 0, 0, tile);
}
