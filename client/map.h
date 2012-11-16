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

#ifndef MAP_H
#define MAP_H

#include "game.h"

int map_grid_lines;
DARNIT_LINE *powergrid;
int powergrid_lines;

DARNIT_MAP *map;
int map_w, map_h;
int home_x, home_y;

struct MAP_GRID_CHUNK {
	DARNIT_LINE *lines;
	int size;
} *map_grid_chunk;
int map_grid_chunks;

unsigned int minimap_data[(SIDEBAR_WIDTH-8)*(SIDEBAR_WIDTH-8)];
DARNIT_LINE *minimap_viewport;

static const unsigned int minimap_colors[]={
	0xFF7F007F,
	0xFF000000,
	
	0xFF0000FF,
	0xFFFF0000,
	0xFF00FF00,
	0xFF00FFFF,
	
	0xFFFFFF,
	0xFF00007F,
};

struct MAP_SELECTED {
	DARNIT_LINE *border;
	int index;
	int building;
} map_selected;

void map_init(char *filename);
void map_close(DARNIT_MAP *map);
void map_update_grid();
void map_calculate_powergrid();
void map_building_place(int index, int player, int building);
void map_set_home(int index);
int map_get_building_health(int index);
int map_get_building_shield(int index);
void map_set_building_health(int index, unsigned int health);
void map_set_building_shield(int index, unsigned int shield);
void map_set_tile_attributes(int index, int attrib);
void map_select_building(int index);
void map_select_nothing();
int map_selected_building();
int map_selected_index();
void map_clear_fow();
void map_draw(int draw_powergrid);
void map_minimap_update_viewport();
void map_minimap_render(UI_WIDGET *widget);
//void map_minimap_update();
void map_minimap_update(DARNIT_TILESHEET *ts, int w, int h, int show_fow);

#endif
