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
 * along with Muon.  If not, see <http://www.gnu.map_selected.circleorg/licenses/>.
 */

#include "muon.h"
#include "view.h"
#include "client.h"
#include "game.h"
#include "map.h"
#include "intmath.h"

#define MAP_INDEX(x, y) ((y)*toplayer_tilemap->w+(x))

void map_init(char *filename) {
	int w, h;
	unsigned int i, j;
	
	powergrid=NULL;
	powergrid_lines=0;
	
	home_x=home_y=-1;
	
	map_selected.border=d_render_line_new(4, 1);
	map_selected.index=-1;
	map_selected.owner=-1;
	map_selected.building=0;
	map_selected.circle=NULL;
	
	map_grid_chunk=NULL;
	map_grid_chunks=0;
	
	game_attacklist=NULL;
	for(i=0; i<MAX_PLAYERS; i++) {
		game_attacklist_render[i].length=0;
		game_attacklist_render[i].lines=NULL;
	}
	
	map=d_map_load(filename);
	map_w=map->layer->tilemap->w*map->layer->tile_w;
	map_h=map->layer->tilemap->h*map->layer->tile_h;
	d_render_tilesheet_geometrics(map->layer->ts, &w, &h, NULL, NULL);
	tilesx=w/map->layer->tile_w;
	tilesy=h/map->layer->tile_h;
	
	DARNIT_MAP_LAYER *building_layer=&map->layer[map->layers-2];
	d_render_line_move(map_selected.border, 0, 0, 0, building_layer->tile_w, 0);
	d_render_line_move(map_selected.border, 1, 0, building_layer->tile_h, building_layer->tile_w, building_layer->tile_h);
	d_render_line_move(map_selected.border, 2, 0, 0, 0, building_layer->tile_h);
	d_render_line_move(map_selected.border, 3, building_layer->tile_w, 0, building_layer->tile_w, building_layer->tile_h);
	
	minimap_viewport=d_render_line_new(4, 1);
	
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
	
	//TODO: move somewhere else
	//Update abilities
	ability[0].ready=100;
	ability[0].button->enabled=1;
	ability[1].ready=-1;
	ability[1].button->enabled=0;
	ability[2].ready=-1;
	ability[2].button->enabled=0;
}

void map_close(/*DARNIT_MAP *map*/) {
	if(!map)
		return;
	int i;
	map_selected.border=d_render_line_free(map_selected.border);
	map_selected.circle=d_render_circle_free(map_selected.circle);
	powergrid=d_render_line_free(powergrid);
	minimap_viewport=d_render_line_free(minimap_viewport);
	map=d_map_unload(map);
	for(i=0; i<map_grid_chunks; i++)
		d_render_line_free(map_grid_chunk[i].lines);
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
		d_render_line_free(map_grid_chunk[i].lines);
	cols=(building_tilemap->w+(building_tilemap->w%8>0?8:0))/8;
	rows=(building_tilemap->h+(building_tilemap->h%8>0?8:0))/8;
	map_grid_chunks=cols*rows;
	free(map_grid_chunk);
	if((map_grid_chunk=malloc(map_grid_chunks*sizeof(struct MAP_GRID_CHUNK)))==NULL)
		return;
	for(i=0; i<map_grid_chunks; i++) {
		map_grid_chunk[i].lines=d_render_line_new(128, 1);
		map_grid_chunk[i].size=0;
	}
	
	int chunk;
	for(y=0; y<building_tilemap->h; y++) {
		chunk=(y/8)*cols;
		for(x=0; x<building_tilemap->w; x++) {
			if(x+1<building_tilemap->w&&!((building_tilemap->data[y*building_tilemap->w+x]&(1<<17))&(building_tilemap->data[y*building_tilemap->w+x+1]&(1<<17)))) {
				d_render_line_move(map_grid_chunk[chunk].lines, map_grid_chunk[chunk].size, (x+1)*tile_w, y*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				map_grid_chunk[chunk].size++;
			}
			if(y+1<building_tilemap->w&&!((building_tilemap->data[y*building_tilemap->w+x]&(1<<17))&(building_tilemap->data[(y+1)*building_tilemap->w+x]&(1<<17)))) {
				d_render_line_move(map_grid_chunk[chunk].lines, map_grid_chunk[chunk].size, x*tile_w, (y+1)*tile_h, (x+1)*tile_w, (y+1)*tile_h);
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
	
	int tile_w=toplayer->tile_w;
	int tile_h=toplayer->tile_h;

	if(powergrid)
		d_render_line_free(powergrid);
	powergrid=d_render_line_new(toplayer_tilemap->w*toplayer_tilemap->h*3, 1);
	
	int x, y, i=0;
	for(y=0; y<toplayer_tilemap->h; y++) {
		for(x=0; x<toplayer_tilemap->w; x++) {
			if(x+1<toplayer_tilemap->w&&(toplayer_data[MAP_INDEX(x, y)]^toplayer_data[MAP_INDEX(x+1, y)])&0x1000000) {
				d_render_line_move(powergrid, i, (x+1)*tile_w, y*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				i++;
			}
			if(y+1<toplayer_tilemap->h&&(toplayer_data[MAP_INDEX(x, y)]^toplayer_data[MAP_INDEX(x, y+1)])&0x1000000) {
				d_render_line_move(powergrid, i, x*tile_w, (y+1)*tile_h, (x+1)*tile_w, (y+1)*tile_h);
				i++;
			}
		}
	}
	powergrid_lines=i;
}

void map_building_place(int index, int player, int building) {
	map->layer[map->layers-2].tilemap->data[index]=(building==BUILDING_CONTROL_POINT)?5:(building!=0)*(((player+1)*tilesx)+building-1)|(map->layer[map->layers-2].tilemap->data[index]&(1<<17));
	if(building==BUILDING_ATTACKER||building==BUILDING_SCOUT) {
		game_attacklist_add(index);
	} else if(building==BUILDING_NONE) {
		game_attacklist_remove(index);
		game_attacklist_untarget(index);
		
		if(index==map_selected_index())
			map_select_nothing();
	}
}

void map_building_clear() {
	DARNIT_TILEMAP *building_tilemap=map->layer[map->layers-2].tilemap;
	unsigned int i;
	for(i=0; i<building_tilemap->w*building_tilemap->h; i++)
		building_tilemap->data[i]&=~0xFFF;
}

int map_isset_home() {
	return !(home_x==-1&&home_y==-1);
}

void map_set_home(int index) {
	home_y=index/map->layer[map->layers-2].tilemap->w;
	home_x=index%map->layer[map->layers-2].tilemap->w;
}

int map_get_home() {
	DARNIT_TILEMAP *toplayer_tilemap=map->layer[map->layers-1].tilemap;
	return map_isset_home()?MAP_INDEX(home_x, home_y):-1;
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

unsigned int map_set_tile_attributes(int index, int attrib) {
	int i;
	unsigned int update=0;
	switch(attrib) {
		case MSG_TILE_ATTRIB_FOW_CLEAR:
			map->layer[map->layers-1].tilemap->data[index]&=~0xFFF;
			update|=1<<(map->layers-1);
			break;
		case MSG_TILE_ATTRIB_FOW_SET:
			map->layer[map->layers-1].tilemap->data[index]=(map->layer[map->layers-1].tilemap->data[index]&~0xFFF)|0x1;
			game_attacklist_untarget(index);
			update|=1<<(map->layers-1);
			break;
		case MSG_TILE_ATTRIB_POWER_CLEAR:
			map->layer[map->layers-1].tilemap->data[index]&=~0x1000000;
			update|=1<<(map->layers-1);
			break;
		case MSG_TILE_ATTRIB_POWER_SET:
			map->layer[map->layers-1].tilemap->data[index]|=0x1000000;
			update|=1<<(map->layers-1);
			break;
		case MSG_TILE_ATTRIB_TERRAIN_CLEAR:
			for(i=map->layers-2; i>=0; i--)  {
				map->layer[i].tilemap->data[index]=0;
				update|=1<<(i);
			}
			map->layer[map->layers-2].tilemap->data[index]|=(1<<17);
			break;
		case MSG_TILE_ATTRIB_TERRAIN_SET:
			//TODO: fix if ever needed
			/*for(i=map->layers-3; i>=0; i--) {
				map->layer[i].tilemap->data[index]=lololol;
				update|=1<<(i);
			}*/
			break;
	}
	return update;
}

void map_select_building(int index) {
	map_selected.index=index;
	int selected_building=(map->layer[map->layers-2].tilemap->data[index]&0xFFFF);
	map_selected.owner=selected_building/tilesx-1;
	map_selected.building=selected_building%tilesx+1;
	
	if(map_selected.owner<0||map_selected.owner>players-1) {
		map_selected.building=0;
		map_selected.index=-1;
		map_selected.owner=-1;
	}
	
	if(map_selected.building&&building[map_selected.building].range) {
		map_selected.circle=d_render_circle_new(32, 1);
		int w=map->layer[map->layers-2].tilemap->w;
		//int h=map->layer[map->layers-2].tilemap->h;
		int tile_w=map->layer[map->layers-2].tile_w;
		int tile_h=map->layer[map->layers-2].tile_h;
		int x=index%w;
		int y=index/w;
		d_render_circle_move(map_selected.circle, x*tile_w+tile_w/2, y*tile_h+tile_h/2, building[map_selected.building].range*tile_w);
	} else
		map_selected.circle=d_render_circle_free(map_selected.circle);
	game_update_building_status();
}

void map_select_nothing() {
	map_selected.building=0;
	map_selected.index=-1;
	map_selected.owner=-1;
	game_update_building_status();
}

int map_selected_building() {
	return map_selected.building;
}

int map_selected_index() {
	return map_selected.index;
}

int map_selected_owner() {
	return map_selected.owner;
}

void map_clear_fow() {
	unsigned int *d=map->layer[map->layers-1].tilemap->data;
	int w=map->layer[map->layers-1].tilemap->w;
	int h=map->layer[map->layers-1].tilemap->h;
	int i;
	for(i=0; i<w*h; i++) {
		d[i]&=~0xFFF;
	}
	d_tilemap_recalc(map->layer[map->layers-1].tilemap);
}

void map_flare_add(int index, int player, unsigned int duration, unsigned int radius) {
	int w=map->layer[map->layers-2].tilemap->w;
	int h=map->layer[map->layers-2].tilemap->h;
	int tile_w=map->layer[map->layers-2].tile_w;
	int tile_h=map->layer[map->layers-2].tile_h;
	int x=index%w;
	int y=index/w;
	
	struct MAP_FLARE_LIST **l;
	for(l=&map_flares; *l; l=&((*l)->next));
	*l=malloc(sizeof(struct MAP_FLARE_LIST));
	(*l)->circle[0]=d_render_circle_new(16, 1);
	d_render_circle_move((*l)->circle[0], x*tile_w+tile_w/2, y*tile_h+tile_h/2, radius/4);
	(*l)->circle[1]=d_render_circle_new(16, 1);
	d_render_circle_move((*l)->circle[1], x*tile_w+tile_w/2, y*tile_h+tile_h/2, radius/2);
	(*l)->circle[2]=d_render_circle_new(24, 1);
	d_render_circle_move((*l)->circle[2], x*tile_w+tile_w/2, y*tile_h+tile_h/2, 3*radius/4);
	(*l)->circle[3]=d_render_circle_new(24, 1);
	d_render_circle_move((*l)->circle[3], x*tile_w+tile_w/2, y*tile_h+tile_h/2, radius);
	
	(*l)->minimap_circle=d_render_circle_new(6, 1);
	d_render_circle_move((*l)->minimap_circle, game_sidebar_minimap->x+(x*(game_sidebar_minimap->w))/(w), game_sidebar_minimap->y+(y*(game_sidebar_minimap->h))/(h), 3);
	
	(*l)->index=0;
	(*l)->player=player;
	(*l)->duration=duration;
	(*l)->created=d_time_get();
	(*l)->next=NULL;
}

void map_flare_draw() {
	struct MAP_FLARE_LIST **ll, *l;
	for(ll=&map_flares; *ll; ll=&((*ll)->next)) {
		l=*ll;
		if(d_time_get()>l->created+l->duration) {
			*ll=l->next;
			int i;
			for(i=0; i<4; i++)
				d_render_circle_free(l->circle[i]);
			d_render_circle_free(l->minimap_circle);
			free(l);
			if(*ll)
				continue;
			else
				break;
		}
		d_render_tint(player_color[l->player].r, player_color[l->player].g, player_color[l->player].b, 255);
		d_render_circle_draw(l->circle[l->index/10]);
		if(l->index>=20) {
			d_render_offset(0, 0);
			d_render_circle_draw(l->minimap_circle);
			d_render_offset(map->cam_x, map->cam_y);
		}
		l->index++;
		if(l->index>=40)
			l->index=0;
		
	}
}

void map_draw(int draw_powergrid) {
	int i;
	for(i=0; i<map->layers-2; i++)
		d_tilemap_draw(map->layer[i].tilemap);
	if(config.alpha)
		d_render_blend_enable();
	d_tilemap_draw(map->layer[map->layers-2].tilemap);
	d_render_blend_disable();
	d_tilemap_draw(map->layer[map->layers-1].tilemap);
	
	d_render_offset(map->cam_x, map->cam_y);
	if(config.grid) {
		d_render_tint(18, 18, 18, 255);
		for(i=0; i<map_grid_chunks; i++)
			if(map_grid_chunk[i].lines)
				d_render_line_draw(map_grid_chunk[i].lines, map_grid_chunk[i].size);
		d_render_tint(255, 255, 255, 255);
	}
	
	d_tilemap_draw(map->layer[map->layers-1].tilemap);
	
	if(powergrid&&(draw_powergrid||config.powergrid))
		d_render_line_draw(powergrid, powergrid_lines);
		
	map_flare_draw();
	
	if(map_selected.index>-1&&map_selected.building) {
		int x=map->layer[map->layers-2].tile_w*(map_selected.index%map->layer[map->layers-2].tilemap->w);
		int y=map->layer[map->layers-2].tile_h*(map_selected.index/map->layer[map->layers-2].tilemap->w);
		d_render_tint(player_color[map_selected.owner].r, player_color[map_selected.owner].r, player_color[map_selected.owner].r, 255);
		if(map_selected.circle)
			d_render_circle_draw(map_selected.circle);
		d_render_offset(map->cam_x-x, map->cam_y-y);
		d_render_line_draw(map_selected.border, 4);
	}
	d_render_tint(255, 255, 255, 255);
	d_render_offset(0, 0);
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
	d_render_line_move(minimap_viewport, 0, x1, y1, x2, y1);
	d_render_line_move(minimap_viewport, 1, x1, y2, x2, y2);
	d_render_line_move(minimap_viewport, 2, x1, y1, x1, y2);
	d_render_line_move(minimap_viewport, 3, x2, y1, x2, y2);
}

void map_minimap_render(UI_WIDGET *widget) {
	//override for imagebox render, to render viewport border on minimap
	ui_imageview_render(widget);
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(255, 255, 255, 255);
	d_render_line_draw(minimap_viewport, 4);
	struct MAP_FLARE_LIST *l;
	for(l=map_flares; l; l=l->next)
		if(l->index>=20) {
			d_render_tint(player_color[l->player].r, player_color[l->player].r, player_color[l->player].r, 255);
			d_render_circle_draw(l->minimap_circle);
		}
	d_render_tint(r, g, b, a);
}

void map_minimap_clear(DARNIT_TILESHEET *ts, int w, int h) {
	int i;
	for(i=0; i<w*h; i++)
		minimap_data[i]=0;
	d_render_tilesheet_update(ts, 0, 0, w, h, minimap_data);
}

void map_minimap_update(DARNIT_TILESHEET *ts, int w, int h, int show_fow) {
	//This is really slow and retarded, but it works
	int x, y;
	DARNIT_TILEMAP *building_tilemap=map->layer[map->layers-2].tilemap;
	DARNIT_TILEMAP *fow_tilemap=map->layer[map->layers-1].tilemap;
	for(y=0; y<(h); y++)
		for(x=0; x<(h); x++) {
			int index=(y*(fow_tilemap->h)/(h))*(fow_tilemap->w)+(x*(fow_tilemap->w))/(w);
			minimap_data[y*(w)+x]=minimap_colors[((fow_tilemap->data[index]&0xFFF)!=1||!show_fow)*(((map->layer[map->layers-2].tilemap->data[index]&(1<<17))==0)?1:7)];
		}
	
	for(y=0; y<building_tilemap->h; y++)
		for(x=0; x<building_tilemap->w; x++)
			if((fow_tilemap->data[(y*building_tilemap->w)+x]&0xFFF)==0||!show_fow) {
				if((building_tilemap->data[(y*building_tilemap->w)+x]&0xFFF)==5)
					minimap_data[(y*(h))/(building_tilemap->h)*(w)+(x*(w))/(building_tilemap->w)]=minimap_colors[6];
				else if((building_tilemap->data[(y*building_tilemap->w)+x]&0xFFF)>0)
					minimap_data[(y*(h))/(building_tilemap->h)*(w)+(x*(w))/(building_tilemap->w)]=minimap_colors[((building_tilemap->data[(y*building_tilemap->w)+x])&0xFFF)/tilesx+1];
			}
	
	d_render_tilesheet_update(ts, 0, 0, w, h, minimap_data);
}
