#include "muon.h"
#include "view.h"
#include "client.h"
#include "map.h"

#define MAP_INDEX(x, y) ((y)*toplayer_tilemap->w+(x))

void map_init(char *filename) {
	powergrid=NULL;
	powergrid_lines=0;
	
	map_selected.border=darnitRenderLineAlloc(4, 1);
	map_selected.index=-1;
	map_selected.building=0;
	
	map=darnitMapLoad(filename);
	map_w=map->layer->tilemap->w*map->layer->tile_w;
	map_h=map->layer->tilemap->h*map->layer->tile_h;
	darnitRenderLineFree(map_border);
	map_border=darnitRenderLineAlloc(4, 1);
	darnitRenderLineMove(map_border, 0, 0, 0, map_w, 0);
	darnitRenderLineMove(map_border, 1, 0, 0, 0, map_h);
	darnitRenderLineMove(map_border, 2, map_w, 0, map_w, map_h);
	darnitRenderLineMove(map_border, 3, 0, map_h, map_w, map_h);
	DARNIT_MAP_LAYER *building_layer=&map->layer[map->layers-2];
	darnitRenderLineMove(map_selected.border, 0, 0, 0, building_layer->tile_w, 0);
	darnitRenderLineMove(map_selected.border, 1, 0, building_layer->tile_h, building_layer->tile_w, building_layer->tile_h);
	darnitRenderLineMove(map_selected.border, 2, 0, 0, 0, building_layer->tile_h);
	darnitRenderLineMove(map_selected.border, 3, building_layer->tile_w, 0, building_layer->tile_w, building_layer->tile_h);
}

void map_calculate_powergrid() {
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

void map_building_place(int index, int player, int building) {
	map->layer[map->layers-2].tilemap->data[index]=(building==BUILDING_BUILDSITE)?5:(building!=0)*(player*8+building+7);
}

void map_set_home(int index) {
	home_y=index/map->layer[map->layers-2].tilemap->w;
	home_x=index%map->layer[map->layers-2].tilemap->w;
}

int map_get_building_health(int index) {
	int health=(map->layer[map->layers-2].tilemap->data[index]>>18)&0xFE;
	return health>100?100:health;
}

int map_get_building_shield(int index) {
	int shield=(map->layer[map->layers-2].tilemap->data[index]>>24)&0xFE;
	return shield>100?100:shield;
}

void map_set_building_health(int index, int health) {
	if(health<0||health>100)
		return;
	map->layer[map->layers-2].tilemap->data[index]|=health<<18;
	darnitRenderTilemapRecalculate(map->layer[map->layers-2].tilemap);
}

void map_set_building_shield(int index, int shield) {
	if(shield<0||shield>100)
		return;
	map->layer[map->layers-2].tilemap->data[index]|=shield<<25;
	darnitRenderTilemapRecalculate(map->layer[map->layers-2].tilemap);
}

void map_set_tile_attributes(int index, int attrib) {
	int layerbits;
	layerbits=((attrib&MSG_TILE_ATTRIB_FOW)==MSG_TILE_ATTRIB_FOW)|(map->layer[map->layers-1].tilemap->data[index]&0x1000000);
	layerbits|=(((attrib&MSG_TILE_ATTRIB_POWER)==MSG_TILE_ATTRIB_POWER)<<24);
	map->layer[map->layers-1].tilemap->data[index]=layerbits;
}

void map_select_building(int offset) {
	map_selected.index=offset;
	int selected_building=map->layer[map->layers-2].tilemap->data[offset]&0xFFFF;
	selected_building-=(player_id+1)*8-1;
	if(selected_building<0||selected_building>7)
		selected_building=0;
	map_selected.building=selected_building;
}

int map_selected_building() {
	return map_selected.building;
}

int map_selected_index() {
	return map_selected.index;
}

void map_draw(int draw_powergrid) {
	int i;
	for(i=0; i<map->layers; i++)
		darnitRenderTilemap(map->layer[i].tilemap);
	darnitRenderOffset(map->cam_x, map->cam_y);
	if(map_border)
		darnitRenderLineDraw(map_border, 4);
	if(powergrid&&draw_powergrid)
		darnitRenderLineDraw(powergrid, powergrid_lines);
	if(map_selected.index>-1&&map_selected.building) {
		int x=map->layer[map->layers-2].tile_w*(map_selected.index%map->layer[map->layers-2].tilemap->w);
		int y=map->layer[map->layers-2].tile_h*(map_selected.index/map->layer[map->layers-2].tilemap->w);
		//printf("(%i, %i) (%i, %i)\n", x, y, map->cam_x, map->cam_y);
		darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
		darnitRenderOffset(map->cam_x-x, map->cam_y-y);
		darnitRenderLineDraw(map_selected.border, 4);
		darnitRenderTint(1, 1, 1, 1);
	}
	darnitRenderOffset(0, 0);
}
