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

#ifndef MAPED_MAP_H
#define MAPED_MAP_H

#include <stdint.h>
 
#define LDI_MAGIC 0x83B3661B
#define LDI_VERSION 0xBBA77ABC
#define LDI_FILENAME_LEN 128

typedef struct {
	uint32_t magic;
	uint32_t version;
	uint32_t files;
} LDI_MAIN;


typedef struct {
	char name[LDI_FILENAME_LEN];
	uint32_t pos;
	uint32_t len;
	uint32_t pad;
} LDI_ENTRY;

#define LDMZ_MAGIC 0xFF00E00E
#define LDMZ_VERSION 0x55555555
struct LDMZ {
	struct LDMZ_HEADER {
		uint32_t magic;
		uint32_t version;
		uint32_t stringtable_size;
		uint32_t stringtable_zsize;
		uint32_t refs_size;
		uint32_t refs_zsize;
		uint32_t layers;
		uint32_t layer_headers_zsize;
		uint32_t objects;
		uint32_t objects_zsize;
		uint32_t refs;
		uint32_t main_ref_index;
	} header;
	unsigned char *stringtablez;
	unsigned char *refsz;
	struct LDMZ_OBJECT {
		uint32_t x;
		uint32_t y;
		uint32_t layer;
		uint32_t ref_index;
	} *object;
	unsigned char *objectz;
	struct LDMZ_LAYER {
		uint32_t width;
		uint32_t height;
		uint32_t offset_x;
		uint32_t offset_y;
		uint32_t zsize;
		uint32_t ref_index;
		uint32_t tile_w;
		uint32_t tile_h;
		
		unsigned char *dataz;
	} *layer;
	unsigned char *layer_headerz;
};

typedef struct MAP_PROPERTY {
	const char *key;
	const char *value;
	struct MAP_PROPERTY *next;
} MAP_PROPERTY;

typedef struct {
	DARNIT_MAP *map;
	MAP_PROPERTY *stringtable;
	unsigned char *tilesheet;
	uint32_t tilesheet_size;
	int w;
	int h;
	char sizestring[8];
} MAP;

MAP *map;

MAP *map_new(unsigned int width, unsigned int height, unsigned int terrain_layers, DARNIT_TILESHEET *ts);
DARNIT_MAP *map_new_palette(unsigned int width, unsigned int height, DARNIT_TILESHEET *ts);
MAP *map_load(const char *filename);
MAP *map_close(MAP *map);
void map_prop_set_or_add(MAP *map, const char *key, const char *value);
const char *map_prop_get(MAP *map, const char *key);
void map_save(MAP *map, const char *filename);

void map_tile_set(DARNIT_TILEMAP *tilemap, int x, int y, int mirrorx, int mirrory, unsigned int tile);

#endif
