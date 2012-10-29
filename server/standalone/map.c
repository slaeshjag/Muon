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
	FILE *fp, *new;
	LDI_HEADER ldih;
	LDI_FILE ldif;
	char dir[128];
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
		if (strcmp("maps/map.ldmz", ldif.name) != 0)
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
	return map->layer->tilemap->data;
}


LDMZ_MAP *ldmzFree(LDMZ_MAP *map) {
	return mapDestroy(map);
}


const char *ldmzFindProp(LDMZ_MAP *map, const char *key) {
	return mapPropSearch(map->prop, key);
}
