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

#ifndef GAME_H
#define GAME_H

#define SIDEBAR_WIDTH 128
#define SCROLL_OFFSET 8
#define SCROLL_SPEED 4

#define PLACE_FLARE -2
#define PLACE_NUKE -3
#define PLACE_RADAR -4

struct UI_PANE_LIST panelist_game_sidebar;
UI_WIDGET *game_sidebar_minimap;
UI_WIDGET *game_sidebar_button_build[5];
UI_WIDGET *game_sidebar_label_build[5];
UI_WIDGET *game_sidebar_progress_build;
UI_WIDGET *game_sidebar_progress_shield;
UI_WIDGET *game_sidebar_progress_health;

struct UI_PANE_LIST panelist_game_specialbar;
UI_WIDGET *game_specialbar_button_build[3];
UI_WIDGET *game_specialbar_label_build[3];

int building_place;
int building_ready;
int building_cancel;

struct BUILDING {
	int range;
} building[16];

struct ABILITY {
	UI_WIDGET *button;
	DARNIT_TILESHEET *icon;
	const char *name;
	void (*action)();
	int delay;
} ability[3];
struct UI_PANE_LIST panelist_game_abilitybar;

struct GAME_ATTACKLIST {
	int index;
	int target;
	struct GAME_ATTACKLIST *next;
} *game_attacklist;

int game_attacklist_length;
DARNIT_LINE *game_attacklist_lines;
unsigned int game_attacklist_blink_semaphore;

void game_view_init();
void game_abilitybar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_sidebar_minimap_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_key_press(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_move(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_update_building_status();
void game_view_scroll_to(int x, int y);
void game_set_building_progress(int building, int progress);
void game_reset_building_progress();
void game_set_building_ready(int building);

void game_attacklist_lines_recalculate();
void game_attacklist_add(int index);
void game_attacklist_remove(int index);
void game_attacklist_clear();
void game_attacklist_untarget(int target);
void game_attacklist_target(int index, int target);

void game_abilitybar_icon_render(UI_WIDGET *widget);

void game_view_draw();
void game_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
