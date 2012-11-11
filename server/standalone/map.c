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

#include "ldmz.h"

#define	LDI_MAGIC			0x83B3661B
#define	LDI_VERSION			0xBBA77ABC

typedef struct {
	unsigned int			magic;
	unsigned int			version;
	unsigned int			files;
} LDI_HEADER;

typedef struct {
	char				name[128];
	unsigned int			pos;
	unsigned int			length;
	unsigned int			pad;
} LDI_FILE;


LDMZ_MAP *ldmzLoad(const char *fname) {
	FILE *fp;
	LDI_HEADER ldih;
	LDI_FILE ldif;
	int i;
	off_t start;
	void *map;

	if ((fp = fopen(fname, "rb")) == NULL) {
		fprintf(stderr, "Unable to open map %s\n", fname);
		return NULL;
	}

	fread(&ldih, sizeof(ldih), 1, fp);
	utilBlockToHostEndian((unsigned int *) &ldih, 3);
	if (ldih.magic != LDI_MAGIC || ldih.version != LDI_VERSION) {
		fprintf(stderr, "'%s' is an invalid map file\n", fname);
		return NULL;
	}

	start = sizeof(LDI_HEADER) + sizeof(LDI_FILE) * ldih.files;
	for (i = 0; i < ldih.files; i++) {
		fread(&ldif, sizeof(ldif), 1, fp);
		if (strcmp("mapdata/map.ldmz", ldif.name) != 0)
			continue;
		utilBlockToHostEndian(&ldif.pos, 3);
		fseek(fp, start + ldif.pos, SEEK_SET);
		map = mapLoad(fp);
		fclose(fp);
		return map;
	}

	fprintf(stderr, "Unable to find a valid map in %s", fname);
	return NULL;
}


void ldmzGetSize(LDMZ_MAP *map, int *x, int *y) {
	*x = map->layer->tilemap->w;
	*y = map->layer->tilemap->h;

	return;
}

unsigned int *ldmzGetData(LDMZ_MAP *map) {
	return map->layer[map->layers-2].tilemap->data;
}


LDMZ_MAP *ldmzFree(LDMZ_MAP *map) {
	return mapDestroy(map);
}


const char *ldmzFindProp(LDMZ_MAP *map, const char *key) {
	return mapPropSearch(map->prop, key);
}

