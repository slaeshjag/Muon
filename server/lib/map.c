#include "server.h"
#include <darnit/darnit.h>

void *ldmzLoad(const char *fname) {
	void *ret;
	darnitFSMount(fname);
	ret = darnitMapLoad("maps/map.ldmz");
	darnitFSUnmount(fname);

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
	
	return dmap->layer->tilemap->data;
}


void *ldmzFree(void *map) {
	return darnitMapUnload(map);
}
