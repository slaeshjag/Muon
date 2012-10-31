#include "muon.h"
#include "view.h"

#define MAP_INDEX(x, y) ((y)*toplayer_tilemap->w+(x))

void engine_calculate_powergrid() {
	DARNIT_MAP_LAYER *toplayer=&map->layer[map->layers-1];
	DARNIT_TILEMAP *toplayer_tilemap=map->layer[map->layers-1].tilemap;
	unsigned int *toplayer_data=map->layer[map->layers-1].tilemap->data;
	//int *building_tilemap=map->layer[map->layers-2].tilemap;
	
	int tile_w=toplayer->tile_w;
	int tile_h=toplayer->tile_h;

	if(powergrid)
		darnitRenderLineFree(powergrid);
	powergrid=darnitRenderLineAlloc(toplayer_tilemap->w*toplayer_tilemap->h*3, 1);
	
	int x, y, i=0;
	for(y=0; y<toplayer_tilemap->h-1; y++) {
		for(x=0; x<toplayer_tilemap->w-1; x++) {
			if((toplayer_data[MAP_INDEX(x, y)]^toplayer_data[MAP_INDEX(x+1, y)])&0x1000000) {
				darnitRenderLineMove(powergrid, i, (x+1)*tile_w, y*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				i++;
			}
			if((toplayer_data[MAP_INDEX(x, y)]^toplayer_data[MAP_INDEX(x, y+1)])&0x1000000) {
				darnitRenderLineMove(powergrid, i, x*tile_w, (y+1)*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				i++;
			}
		}
	}
	powergrid_lines=i;
}

void engine_move_powergrid(int cam_x, int cam_y) {
	int i;
	int x1, x2, y1, y2;
	if(!powergrid)
		return;
	for(i=0; i<powergrid_lines; i++) {
		darnitRenderLineGet(powergrid, i, &x1, &y1, &x2, &y2);
		darnitRenderLineMove(powergrid, i, x1-cam_x, y1-cam_y, x2-cam_x, y2-cam_y);
	}
}
