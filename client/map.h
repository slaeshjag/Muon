#ifndef MAP_H
#define MAP_H

#include "game.h"

DARNIT_LINE *map_border;
DARNIT_LINE *powergrid;
int powergrid_lines;

DARNIT_MAP *map;
int map_w, map_h;
int home_x, home_y;

unsigned int minimap_data[(SIDEBAR_WIDTH-8)*(SIDEBAR_WIDTH-8)];
DARNIT_LINE *minimap_viewport;

static const unsigned int minimap_colors[]={
	0xFF000000,
	0xFF7F007F,
	
	0xFF0000FF,
	0xFFFF0000,
	0xFF00FF00,
	0xFF00FFFF,
	
	0xFFFFFF,
};

struct MAP_SELECTED {
	DARNIT_LINE *border;
	int index;
	int building;
} map_selected;

void map_init(char *filename);
void map_close(DARNIT_MAP *map);
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
void map_minimap_update();

#endif
