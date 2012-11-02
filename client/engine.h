#ifndef ENGINE_H
#define ENGINE_H

void engine_calculate_powergrid();

int engine_get_building_health(int index);
int engine_get_building_shield(int index);
void engine_set_building_health(int index, int health);
void engine_set_building_shield(int index, int shield);

#endif
