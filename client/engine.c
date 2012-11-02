#include "muon.h"
#include "view.h"

#define MAP_INDEX(x, y) ((y)*toplayer_tilemap->w+(x))

void engine_calculate_powergrid() {
	DARNIT_MAP_LAYER *toplayer=&map->layer[map->layers-1];
	DARNIT_TILEMAP *toplayer_tilemap=map->layer[map->layers-1].tilemap;
	unsigned int *toplayer_data=map->layer[map->layers-1].tilemap->data;
	//DARNIT_TILEMAP *building_tilemap=map->layer[map->layers-2].tilemap;
	
	int tile_w=toplayer->tile_w;
	int tile_h=toplayer->tile_h;

	if(powergrid)
		darnitRenderLineFree(powergrid);
	powergrid=darnitRenderLineAlloc(toplayer_tilemap->w*toplayer_tilemap->h*3, 1);
	
	int x, y, i=0;
	for(y=0; y<toplayer_tilemap->h; y++) {
		for(x=0; x<toplayer_tilemap->w; x++) {
			if(x+1<toplayer_tilemap->w&&(toplayer_data[MAP_INDEX(x, y)]^toplayer_data[MAP_INDEX(x+1, y)])&0x1000000) {
				darnitRenderLineMove(powergrid, i, (x+1)*tile_w, y*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				i++;
			}
			if(y+1<toplayer_tilemap->h&&(toplayer_data[MAP_INDEX(x, y)]^toplayer_data[MAP_INDEX(x, y+1)])&0x1000000) {
				darnitRenderLineMove(powergrid, i, x*tile_w, (y+1)*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				i++;
			}
		}
	}
	powergrid_lines=i;
}

int engine_get_building_health(int index) {
	int health=(map->layer[map->layers-2].tilemap->data[index]>>18)&0xFE;
	return health>100?100:health;
}

int engine_get_building_shield(int index) {
	int shield=(map->layer[map->layers-2].tilemap->data[index]>>24)&0xFE;
	return shield>100?100:shield;
}

void engine_set_building_health(int index, int health) {
	if(health<0||health>100)
		return;
	map->layer[map->layers-2].tilemap->data[index]|=health<<18;
	darnitRenderTilemapRecalculate(map->layer[map->layers-2].tilemap);
}

void engine_set_building_shield(int index, int shield) {
	if(shield<0||shield>100)
		return;
	map->layer[map->layers-2].tilemap->data[index]|=shield<<25;
	darnitRenderTilemapRecalculate(map->layer[map->layers-2].tilemap);
}
