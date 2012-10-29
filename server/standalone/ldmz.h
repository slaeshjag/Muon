#ifndef __TTMZ_H__
#define	__TTMZ_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

#ifndef DARNIT_MAP
#include "stb_image.h"
#include "util.h"
#endif

#define	LDMZ_MAGIC		0xFF00E00E
#define	LDMZ_VERSION		0x55555555


typedef struct {
	int			w;
	int			h;
	unsigned int		*data;
} LDMZ_TILEMAP;

typedef struct {
	unsigned int		magic;
	unsigned int		version;
	unsigned int		strtable_len;
	unsigned int		strtable_zlen;
	unsigned int		strtable_refs_len;
	unsigned int		strtable_refs_zlen;
	unsigned int		layers;
	unsigned int		layer_zlen;
	unsigned int		objects;
	unsigned int		object_zlen;
	unsigned int		refs;
	unsigned int		main_ref;
} LDMZ_FILE_MAP;


typedef struct {
	int			key;
	int			value;
} LDMZ_FILE_STRTABLE_REF;


typedef struct {
	unsigned int		layer_w;
	unsigned int		layer_h;
	int			layer_offset_x;
	int			layer_offset_y;
	unsigned int		layer_zlen;
	unsigned int		prop_ref;
	unsigned int		tile_w;
	unsigned int		tile_h;
} LDMZ_FILE_LAYER;


typedef struct {
	unsigned int		x;
	unsigned int		y;
	unsigned int		l;
	unsigned int		ref;
} LDMZ_FILE_OBJECT;


typedef struct {
	const char		*key;
	const char		*value;
} LDMZ_REF;


typedef struct {
	unsigned int		x;
	unsigned int		y;
	unsigned int		l;
	LDMZ_REF		*ref;
} LDMZ_OBJECT;


typedef struct {
	LDMZ_REF		*ref;
	unsigned int		offset_x;
	unsigned int		offset_y;
	unsigned int		tile_w;
	unsigned int		tile_h;
	LDMZ_TILEMAP		*tilemap;
} LDMZ_LAYER;


typedef struct {
	LDMZ_LAYER		*layer;
	unsigned int		layers;
	LDMZ_OBJECT		*object;
	unsigned int		objects;
	LDMZ_REF		*prop;
	void			*stringdata;
	LDMZ_REF		*stringrefs;

	int			cam_x;
	int			cam_y;
} LDMZ_MAP;


const char *mapPropSearch(LDMZ_REF *ref, const char *key);
const char *mapLayerPropGet(LDMZ_MAP *map, int layer, const char *key);
LDMZ_MAP *mapDestroy(LDMZ_MAP *map);
LDMZ_MAP *mapLoad(FILE *fp);


#endif
