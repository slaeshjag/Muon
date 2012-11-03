#ifndef MAP_H
#define MAP_H

DARNIT_LINE *map_border;
DARNIT_LINE *powergrid;
int powergrid_lines;

struct MAP_SELECTED {
	DARNIT_LINE *border;
	int index;
	int building;
} map_selected;

void map_init(char *filename);
void map_calculate_powergrid();
void map_building_place(int player, int building, int index);
void map_set_home(int index);
int map_get_building_health(int index);
int map_get_building_shield(int index);
void map_set_building_health(int index, int health);
void map_set_building_shield(int index, int shield);
void map_select_building(int offset);
int map_selected_building();
int map_selected_index();
void map_draw(int draw_powergrid);

#endif
