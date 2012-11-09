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
#include "ldmz.h"


LDMZ_TILEMAP *mapNewTilemap(int w, int h) {
	LDMZ_TILEMAP *tm;

	if ((tm = malloc(sizeof(LDMZ_TILEMAP))) == NULL)
		return NULL;
	if ((tm->data = malloc(sizeof(unsigned int) * w * h)) == NULL) {
		free(tm);
		return NULL;
	}

	tm->w = w;
	tm->h = h;
	
	return tm;
}


LDMZ_TILEMAP *mapDeleteTilemap(LDMZ_TILEMAP *tm) {
	free(tm->data);
	free(tm);

	return NULL;
}


const char *mapPropSearch(LDMZ_REF *ref, const char *key) {
	int i;

	for (i = 0; ref[i].key != NULL; i++)
		if (strcmp(ref[i].key, key) == 0)
			return ref[i].value;
	return "NO SUCH KEY";
}


const char *mapPropGet(LDMZ_MAP *map, const char *key) {
	return mapPropSearch(map->prop, key);
}


const char *mapLayerPropGet(LDMZ_MAP *map, int layer, const char *key) {
	if (map->layers <= layer)
		return "LAYER OUT OF BOUND";
	return mapPropSearch(map->layer[layer].ref, key);
}


LDMZ_MAP *mapDestroy(LDMZ_MAP *map) {
	int i;
	
	if (!map)
		return NULL;

	free(map->stringrefs);
	free(map->object);
	free(map->stringdata);
	for (i = 0; i < map->layers; i++)
		mapDeleteTilemap(map->layer[i].tilemap);
	free(map->layer);
	free(map);

	return NULL;
}


LDMZ_MAP *mapLoad(FILE *fp) {
	LDMZ_MAP *map;
	LDMZ_FILE_MAP map_h;
	LDMZ_FILE_STRTABLE_REF *map_r;
	LDMZ_FILE_LAYER *map_l;
	LDMZ_FILE_OBJECT *object;
	void *buff, *tmp;
	char *stringdata;
	int i;

	fread((unsigned int *) &map_h, sizeof(LDMZ_FILE_MAP), 1, fp);
	utilBlockToHostEndian((unsigned int *) &map_h, sizeof(LDMZ_FILE_MAP) >> 2);

	if (map_h.magic != LDMZ_MAGIC || map_h.version != LDMZ_VERSION) {
		fclose(fp);
		return NULL;
	}

	if ((buff = malloc(map_h.strtable_zlen)) == NULL) {
		return NULL;
	}

	stringdata = malloc(map_h.strtable_len);
	map = malloc(sizeof(LDMZ_MAP));
	map->stringrefs = malloc(sizeof(LDMZ_REF) * (map_h.strtable_refs_len / sizeof(LDMZ_FILE_STRTABLE_REF)));
	map_r = malloc(map_h.strtable_refs_len);
	map_l = malloc(sizeof(LDMZ_FILE_LAYER) * map_h.layers);
	object = malloc(map_h.objects * sizeof(LDMZ_OBJECT));
	map->object = malloc(sizeof(LDMZ_OBJECT) * (map_h.objects));
	map->layer = malloc(sizeof(LDMZ_LAYER) * map_h.layers);

	map->stringdata = stringdata;
	map->cam_x = map->cam_y = 0;
	map->layers = map_h.layers;
	map->objects = map_h.objects;

	if (!map || (!stringdata && map_h.strtable_len > 0) || (!map->stringrefs && map_h.strtable_refs_len > 0)
	    || (!map->object && map_h.objects) || (!map->layer && map_h.layers) 
	    || (!map_r && map_h.strtable_refs_len) || (!object && map_h.objects) || (!map_l)) 

		goto error;		/* Down at the bottom of the function */
	
	/**** STRINGS ****/

	fread(buff, map_h.strtable_zlen, 1, fp);
	stbi_zlib_decode_buffer(stringdata, map_h.strtable_len, buff, map_h.strtable_zlen);
	
	/**** REFS ****/

	if ((tmp = realloc(buff, map_h.strtable_refs_zlen)) == NULL)
		goto error;		/* Down at the bottom of the function */
	buff = tmp;
	fread(buff, map_h.strtable_refs_zlen, 1, fp);
	stbi_zlib_decode_buffer((void *) map_r, map_h.strtable_refs_len, buff, map_h.strtable_refs_zlen);
	utilBlockToHostEndian((unsigned int *) map_r, map_h.strtable_refs_len / 4);
	for (i = 0; i < (map_h.strtable_refs_len / sizeof(LDMZ_FILE_STRTABLE_REF)); i++) {
		if (map_r[i].key != -1) {
			map->stringrefs[i].key = &stringdata[map_r[i].key];
			map->stringrefs[i].value = &stringdata[map_r[i].value];
		} else {
			map->stringrefs[i].key = NULL;
			map->stringrefs[i].value = NULL;
		}
	}
	free(map_r);
	map_r = NULL;
	map->prop = &map->stringrefs[map_h.main_ref];

	/**** OBJECTS ****/

	if ((tmp = realloc(buff, map_h.object_zlen)) == NULL && map_h.objects > 0) {
		goto error;		/* Down at the bottom of the function */
	}
	buff = tmp;
	fread(buff, map_h.object_zlen, 1, fp);
	stbi_zlib_decode_buffer((void *) object, sizeof(LDMZ_OBJECT) * map_h.objects, buff, map_h.object_zlen);
	utilBlockToHostEndian((unsigned int *) object, (sizeof(LDMZ_OBJECT) * map_h.objects) >> 2);
	for (i = 0; i < map_h.objects; i++) {
		map->object[i].ref = &map->stringrefs[object[i].ref];
		map->object[i].x = object[i].x;
		map->object[i].y = object[i].y;
		map->object[i].l = object[i].l;
	}
	free(object);
	object = NULL;

	/**** MAP LAYER ****/

	if ((tmp = realloc(buff, map_h.layer_zlen)) == NULL)
		goto error;		/* Down at the bottom of the function */
	
	for (i = 0; i < map_h.layers; i++)
		map->layer[i].tilemap = NULL;
	buff = tmp;
	fread(buff, map_h.layer_zlen, 1, fp);
	stbi_zlib_decode_buffer((void *)map_l, sizeof(LDMZ_FILE_LAYER) * map_h.layers, buff, map_h.layer_zlen);
	utilBlockToHostEndian((unsigned int *) map_l, (sizeof(LDMZ_FILE_LAYER) * map_h.layers) >> 2);
	for (i = 0; i < map_h.layers; i++) {
		map->layer[i].offset_x = map_l[i].layer_offset_x;
		map->layer[i].offset_y = map_l[i].layer_offset_y;
		map->layer[i].tile_w = map_l[i].tile_w;
		map->layer[i].tile_h = map_l[i].tile_h;
		map->layer[i].ref = &map->stringrefs[map_l[i].prop_ref];

		if ((tmp = realloc(buff, map_l[i].layer_zlen)) == NULL) 
			goto error;		/* Down at the bottom of the function */
		buff = tmp;

		if ((map->layer[i].tilemap = mapNewTilemap(map_l[i].layer_w, map_l[i].layer_h)) == NULL)
			goto error;
		fread(buff, map_l[i].layer_zlen, 1, fp);
		stbi_zlib_decode_buffer((void *) map->layer[i].tilemap->data, map_l[i].layer_w * map_l[i].layer_h * 4, buff, map_l[i].layer_zlen);
		utilBlockToHostEndian(map->layer[i].tilemap->data, map_l[i].layer_w * map_l[i].layer_h);
	}
	
	free(map_l);
	free(buff);
	return map;

	error:		/* I know... This is my first goto in C. Ever. <.< */

		for (i = 0; i < map_h.layers; i++)
			mapDeleteTilemap(map->layer[i].tilemap);
		free(map_l);
		free(map_r);
		free(object);
		free(buff);
		mapDestroy(map);
	return NULL;
}
