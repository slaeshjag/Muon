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

#include "muon.h"
#include "view.h"
#include "client.h"
#include "game.h"
#include "map.h"

#define MAP_INDEX(x, y) ((y)*toplayer_tilemap->w+(x))

#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))

void map_init(char *filename) {
	unsigned int i, j;
	
	powergrid=NULL;
	powergrid_lines=0;
	
	home_x=home_y=0;
	
	map_selected.border=darnitRenderLineAlloc(4, 1);
	map_selected.index=-1;
	map_selected.building=0;
	
	map_grid_chunk=NULL;
	map_grid_chunks=0;
	
	game_attacklist=NULL;
	game_attacklist_lines=NULL;
	game_attacklist_length=0;
	
	map=darnitMapLoad(filename);
	map_w=map->layer->tilemap->w*map->layer->tile_w;
	map_h=map->layer->tilemap->h*map->layer->tile_h;
	
	DARNIT_MAP_LAYER *building_layer=&map->layer[map->layers-2];
	darnitRenderLineMove(map_selected.border, 0, 0, 0, building_layer->tile_w, 0);
	darnitRenderLineMove(map_selected.border, 1, 0, building_layer->tile_h, building_layer->tile_w, building_layer->tile_h);
	darnitRenderLineMove(map_selected.border, 2, 0, 0, 0, building_layer->tile_h);
	darnitRenderLineMove(map_selected.border, 3, building_layer->tile_w, 0, building_layer->tile_w, building_layer->tile_h);
	
	minimap_viewport=darnitRenderLineAlloc(4, 1);
	
	for(i=0; i<(SIDEBAR_WIDTH-8)*(SIDEBAR_WIDTH-8); i++)
		((unsigned int *)minimap_data)[i]=0;
	
	for(i=0; i<map->layer->tilemap->w*map->layer->tilemap->w; i++) {
		for(j=0; j<map->layers-2; j++)
			if(map->layer[j].tilemap->data[i]&0xFFF)
				break;
		if(j>=map->layers-2)
			map->layer[map->layers-2].tilemap->data[i]|=1<<17;
	}
	map_update_grid();
}

void map_close(DARNIT_MAP *map) {
	if(!map)
		return;
	int i;
	map_selected.border=darnitRenderLineFree(map_selected.border);
	powergrid=darnitRenderLineFree(powergrid);
	minimap_viewport=darnitRenderLineFree(minimap_viewport);
	map=darnitMapUnload(map);
	for(i=0; i<map_grid_chunks; i++)
		darnitRenderLineFree(map_grid_chunk[i].lines);
	free(map_grid_chunk);
	map_grid_chunk=NULL;
	map_grid_chunks=0;
	
	game_attacklist_clear();
}

void map_update_grid() {
	DARNIT_TILEMAP *building_tilemap=map->layer[map->layers-2].tilemap;
	int i, x, y, cols, rows;
	int tile_w=map->layer[map->layers-2].tile_w;
	int tile_h=map->layer[map->layers-2].tile_h;
	for(i=0; i<map_grid_chunks; i++)
		darnitRenderLineFree(map_grid_chunk[i].lines);
	cols=(building_tilemap->w+(building_tilemap->w%8>0?8:0))/8;
	rows=(building_tilemap->h+(building_tilemap->h%8>0?8:0))/8;
	map_grid_chunks=cols*rows;
	free(map_grid_chunk);
	if((map_grid_chunk=malloc(map_grid_chunks*sizeof(struct MAP_GRID_CHUNK)))==NULL)
		return;
	for(i=0; i<map_grid_chunks; i++) {
		map_grid_chunk[i].lines=darnitRenderLineAlloc(128, 1);
		map_grid_chunk[i].size=0;
	}
	
	int chunk;
	for(y=0; y<building_tilemap->h; y++) {
		chunk=(y/8)*cols;
		for(x=0; x<building_tilemap->w; x++) {
			if(x+1<building_tilemap->w&&!((building_tilemap->data[y*building_tilemap->w+x]&(1<<17))&(building_tilemap->data[y*building_tilemap->w+x+1]&(1<<17)))) {
				darnitRenderLineMove(map_grid_chunk[chunk].lines, map_grid_chunk[chunk].size, (x+1)*tile_w, y*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				map_grid_chunk[chunk].size++;
			}
			if(y+1<building_tilemap->w&&!((building_tilemap->data[y*building_tilemap->w+x]&(1<<17))&(building_tilemap->data[(y+1)*building_tilemap->w+x]&(1<<17)))) {
				darnitRenderLineMove(map_grid_chunk[chunk].lines, map_grid_chunk[chunk].size, x*tile_w, (y+1)*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				map_grid_chunk[chunk].size++;
			}
			if((x+1)%8==0)
				chunk++;
		}
	}
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
	map->layer[map->layers-2].tilemap->data[index]=(building==BUILDING_BUILDSITE)?5:(building!=0)*(player*8+building+7)|(map->layer[map->layers-2].tilemap->data[index]&(1<<17));
	if(building==BUILDING_ATTACKER||building==BUILDING_SCOUT) {
		game_attacklist_add(index);
	} else if(building==BUILDING_NONE) {
		game_attacklist_remove(index);
		game_attacklist_untarget(index);
		if(index==map_selected_index())
			map_select_nothing();
	}
}

void map_set_home(int index) {
	home_y=index/map->layer[map->layers-2].tilemap->w;
	home_x=index%map->layer[map->layers-2].tilemap->w;
}

int map_get_building_health(int index) {
	unsigned int health=(map->layer[map->layers-2].tilemap->data[index]>>18)&0x7F;
	return health>100?100:health;
}

int map_get_building_shield(int index) {
	unsigned int shield=(map->layer[map->layers-2].tilemap->data[index]>>25)&0x7F;
	return shield>100?100:shield;
}

void map_set_building_health(int index, unsigned int health) {
	if(health<0||health>100)
		return;
	map->layer[map->layers-2].tilemap->data[index]=(map->layer[map->layers-2].tilemap->data[index]&0xFE03FFFF)|health<<18;
	if(index==map_selected.index)
		game_update_building_status();
}

void map_set_building_shield(int index, unsigned int shield) {
	if(shield<0||shield>100)
		return;
	map->layer[map->layers-2].tilemap->data[index]=(map->layer[map->layers-2].tilemap->data[index]&0x1FFFFFF)|shield<<25;
	if(index==map_selected.index)
		game_update_building_status();
}

void map_set_tile_attributes(int index, int attrib) {
	switch(attrib) {
		case MSG_TILE_ATTRIB_FOW_CLEAR:
			map->layer[map->layers-1].tilemap->data[index]&=~0xFFF;
			break;
		case MSG_TILE_ATTRIB_FOW_SET:
			map->layer[map->layers-1].tilemap->data[index]=(map->layer[map->layers-1].tilemap->data[index]&~0xFFF)|0x1;
			break;
		case MSG_TILE_ATTRIB_POWER_CLEAR:
			map->layer[map->layers-1].tilemap->data[index]&=~0x1000000;
			break;
		case MSG_TILE_ATTRIB_POWER_SET:
			map->layer[map->layers-1].tilemap->data[index]|=0x1000000;
			break;
	}
}

void map_select_building(int index) {
	map_selected.index=index;
	int selected_building=map->layer[map->layers-2].tilemap->data[index]&0xFFFF;
	selected_building-=(player_id+1)*8-1;
	if(selected_building<0||selected_building>7)
		selected_building=0;
	map_selected.building=selected_building;
}

void map_select_nothing() {
	map_selected.building=0;
	map_selected.index=0;
	game_update_building_status();
}

int map_selected_building() {
	return map_selected.building;
}

int map_selected_index() {
	return map_selected.index;
}

void map_clear_fow() {
	unsigned int *d=map->layer[map->layers-1].tilemap->data;
	int w=map->layer[map->layers-1].tilemap->w;
	int h=map->layer[map->layers-1].tilemap->h;
	int i;
	for(i=0; i<w*h; i++) {
		d[i]&=~0xFFF;
	}
}

void map_draw(int draw_powergrid) {
	int i;
	for(i=0; i<map->layers-1; i++)
		darnitRenderTilemap(map->layer[i].tilemap);
	
	darnitRenderOffset(map->cam_x, map->cam_y);
	if(config.grid) {
		darnitRenderTint(0.07, 0.07, 0.07, 1);
		for(i=0; i<map_grid_chunks; i++)
			if(map_grid_chunk[i].lines)
				darnitRenderLineDraw(map_grid_chunk[i].lines, map_grid_chunk[i].size);
		darnitRenderTint(1, 1, 1, 1);
	}
	
	darnitRenderTilemap(map->layer[map->layers-1].tilemap);
	
	if(powergrid&&draw_powergrid)
		darnitRenderLineDraw(powergrid, powergrid_lines);
	if(map_selected.index>-1&&map_selected.building) {
		int x=map->layer[map->layers-2].tile_w*(map_selected.index%map->layer[map->layers-2].tilemap->w);
		int y=map->layer[map->layers-2].tile_h*(map_selected.index/map->layer[map->layers-2].tilemap->w);
		darnitRenderTint(!(player_id%3), player_id>1, player_id==1, 1);
		darnitRenderOffset(map->cam_x-x, map->cam_y-y);
		darnitRenderLineDraw(map_selected.border, 4);
		darnitRenderTint(1, 1, 1, 1);
	}
	darnitRenderOffset(0, 0);
}

void map_minimap_update_viewport() {
	int x=game_sidebar_minimap->w*map->cam_x/map_w+game_sidebar_minimap->x;
	int y=game_sidebar_minimap->h*map->cam_y/map_h+game_sidebar_minimap->y;
	int w=game_sidebar_minimap->w*platform.screen_w/map_w;
	int h=game_sidebar_minimap->h*platform.screen_h/map_h;
	int x1=MAX(x, game_sidebar_minimap->x);
	int x2=MIN(x+w, game_sidebar_minimap->x+game_sidebar_minimap->w);
	int y1=MAX(y, game_sidebar_minimap->y);
	int y2=MIN(y+h, game_sidebar_minimap->y+game_sidebar_minimap->h);
	darnitRenderLineMove(minimap_viewport, 0, x1, y1, x2, y1);
	darnitRenderLineMove(minimap_viewport, 1, x1, y2, x2, y2);
	darnitRenderLineMove(minimap_viewport, 2, x1, y1, x1, y2);
	darnitRenderLineMove(minimap_viewport, 3, x2, y1, x2, y2);
}

void map_minimap_render(UI_WIDGET *widget) {
	//override for imagebox render, to render viewport border on minimap
	ui_imageview_render(widget);
	float r, g, b, a;
	darnitRenderTintGet(&r, &g, &b, &a);
	darnitRenderTint(1, 1, 1, 1);
	darnitRenderLineDraw(minimap_viewport, 4);
	darnitRenderTint(r, g, b, a);
}

void map_minimap_update() {
	//This is really slow and retarded, but it works
	int x, y;
	DARNIT_TILEMAP *building_tilemap=map->layer[map->layers-2].tilemap;
	DARNIT_TILEMAP *fow_tilemap=map->layer[map->layers-1].tilemap;
	for(y=0; y<(SIDEBAR_WIDTH-8); y++)
		for(x=0; x<(SIDEBAR_WIDTH-8); x++) {
			int index=(y*(fow_tilemap->h)/(SIDEBAR_WIDTH-8))*(fow_tilemap->w)+(x*(fow_tilemap->w))/(SIDEBAR_WIDTH-8);
			minimap_data[y*(SIDEBAR_WIDTH-8)+x]=minimap_colors[((fow_tilemap->data[index]&0xFFF)!=1)*(((map->layer[map->layers-2].tilemap->data[index]&(1<<17))==0)?1:7)];
		}
	
	for(y=0; y<building_tilemap->h; y++)
		for(x=0; x<building_tilemap->w; x++)
			if((fow_tilemap->data[(y*building_tilemap->w)+x]&0xFFF)==0) {
				if(building_tilemap->data[(y*building_tilemap->w)+x]==5)
					minimap_data[(y*(SIDEBAR_WIDTH-8))/(building_tilemap->h)*(SIDEBAR_WIDTH-8)+(x*(SIDEBAR_WIDTH-8))/(building_tilemap->w)]=minimap_colors[6];
				else if((building_tilemap->data[(y*building_tilemap->w)+x]&0xFFF)>0)
					minimap_data[(y*(SIDEBAR_WIDTH-8))/(building_tilemap->h)*(SIDEBAR_WIDTH-8)+(x*(SIDEBAR_WIDTH-8))/(building_tilemap->w)]=minimap_colors[((building_tilemap->data[(y*building_tilemap->w)+x])&0xFFF)/8+1];
			}
	
	UI_PROPERTY_VALUE v;
	v=game_sidebar_minimap->get_prop(game_sidebar_minimap, UI_IMAGEVIEW_PROP_TILESHEET);
	darnitRenderTilesheetUpdate(v.p, 0, 0, SIDEBAR_WIDTH-8, SIDEBAR_WIDTH-8, minimap_data);
}
