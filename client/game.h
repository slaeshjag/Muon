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
#include "client.h"

#define SIDEBAR_WIDTH_MAX 128
#define SCROLL_OFFSET 8
#define SCROLL_SPEED 4

#define PLACE_FLARE -2
#define PLACE_NUKE -3
#define PLACE_RADAR -4

int sidebar_width;

struct UI_PANE_LIST panelist_game_sidebar;
UI_WIDGET *game_sidebar_minimap;
UI_WIDGET *game_sidebar_button_build[5];
UI_WIDGET *game_sidebar_label_build[5];
UI_WIDGET *game_sidebar_progress_build;

struct {
	UI_WIDGET *spacer;
	UI_WIDGET *label_name;
	UI_WIDGET *label_shield;
	UI_WIDGET *progress_shield;
	UI_WIDGET *label_health;
	UI_WIDGET *progress_health;
} game_sidebar_status;

struct UI_PANE_LIST panelist_game_specialbar;
UI_WIDGET *game_specialbar_button_build[3];
UI_WIDGET *game_specialbar_label_build[3];

int building_place;
int building_ready;
int building_cancel;
int ability_place;
int attacker_target;

unsigned int game_time_start;

struct BUILDING {
	const char *name;
	int range;
} building[20];

struct ABILITY {
	UI_WIDGET *button;
	DARNIT_TILESHEET *icon;
	DARNIT_TEXT_SURFACE *text;
	const char *name;
	void (*action)();
	int ready;
} ability[3];
struct UI_PANE_LIST panelist_game_abilitybar;

struct GAME_ATTACKLIST {
	int index;
	int target;
	int owner;
	struct GAME_ATTACKLIST *next;
} *game_attacklist;

struct GAME_ATTACKLIST_RENDER {
	int length;
	DARNIT_LINE *lines;
} game_attacklist_render[MAX_PLAYERS];

//int game_attacklist_length;
//DARNIT_LINE *game_attacklist_lines;
unsigned int game_attacklist_blink_semaphore;

void game_view_init();
void game_abilitybar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_sidebar_minimap_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_key_press(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_move(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_release(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_update_building_status();
void game_view_scroll_to(int x, int y);
void game_set_building_progress(int building, int progress);
void game_reset_building_progress();
void game_set_building_ready(int building);
int game_ability_place(int index);

void game_attacklist_lines_recalculate();
void game_attacklist_add(int index);
void game_attacklist_remove(int index);
void game_attacklist_clear();
void game_attacklist_untarget(int target);
void game_attacklist_target(int index, int target);

void game_abilitybar_icon_render(UI_WIDGET *widget);

void game_view_draw();
void game_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_mouse_target_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
