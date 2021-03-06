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

#ifndef EDITOR_H
#define EDITOR_H

#define SCROLL_OFFSET 8
#define SCROLL_SPEED 4

enum TERRAIN_TOOL {
	TERRAIN_TOOL_NONE=-1,
	TERRAIN_TOOL_BRUSH,
	TERRAIN_TOOL_BUCKET,
	TERRAIN_TOOL_RECTANGLE,
	TERRAIN_TOOLS,
};

enum {
	EDITOR_TOPBAR_BUTTON_MENU,
	EDITOR_TOPBAR_CHECKBOX_MIRRORX,
	EDITOR_TOPBAR_CHECKBOX_MIRRORY,
	EDITOR_TOPBAR_BUTTON_TERRAIN,
	EDITOR_TOPBAR_BUTTON_BUILDINGS,
	EDITOR_TOPBAR_BUTTON_PROPERTIES,
	EDITOR_TOPBAR_BUTTONS,
};

enum {
	EDITOR_SIDEBAR_MENU_LABEL,
	EDITOR_SIDEBAR_MENU_BUTTON_SAVE,
	EDITOR_SIDEBAR_MENU_BUTTON_QUIT,
	EDITOR_SIDEBAR_MENU_WIDGETS,
};

enum {
	EDITOR_SIDEBAR_TERRAIN_LABEL,
	EDITOR_SIDEBAR_TERRAIN_LABEL_LAYERS,
	EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS,
	EDITOR_SIDEBAR_TERRAIN_BUTTON_BRUSH,
	EDITOR_SIDEBAR_TERRAIN_BUTTON_BUCKET,
	EDITOR_SIDEBAR_TERRAIN_BUTTON_RECTANGLE,
	EDITOR_SIDEBAR_TERRAIN_SPACER,
	EDITOR_SIDEBAR_TERRAIN_BUTTON_PALETTE,
	EDITOR_SIDEBAR_TERRAIN_BUTTON_ERASER,
	EDITOR_SIDEBAR_TERRAIN_WIDGETS,
};

enum {
	EDITOR_SIDEBAR_BUILDINGS_LABEL,
	EDITOR_SIDEBAR_BUILDINGS_LABEL_PLAYER,
	EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER,
	EDITOR_SIDEBAR_BUILDINGS_LABEL_BUILDING,
	EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING,
	EDITOR_SIDEBAR_BUILDINGS_SPACER,
	EDITOR_SIDEBAR_BUILDINGS_WIDGETS,
};

enum {
	EDITOR_SIDEBAR_PROPERTIES_LABEL,
	EDITOR_SIDEBAR_PROPERTIES_LABEL_NAME,
	EDITOR_SIDEBAR_PROPERTIES_ENTRY_NAME,
	EDITOR_SIDEBAR_PROPERTIES_LABEL_VERSION,
	EDITOR_SIDEBAR_PROPERTIES_ENTRY_VERSION,
	EDITOR_SIDEBAR_PROPERTIES_LABEL_AUTHOR,
	EDITOR_SIDEBAR_PROPERTIES_ENTRY_AUTHOR,
	EDITOR_SIDEBAR_PROPERTIES_LABEL_PLAYERS,
	EDITOR_SIDEBAR_PROPERTIES_SLIDER_PLAYERS,
	EDITOR_SIDEBAR_PROPERTIES_WIDGETS,
};

struct {
	struct {
		UI_PANE *pane;
		UI_WIDGET *button[EDITOR_TOPBAR_BUTTONS];
	} topbar;
	struct {
		UI_PANE *pane;
		UI_WIDGET *minimap;
		UI_WIDGET *menu[EDITOR_SIDEBAR_MENU_WIDGETS];
		UI_WIDGET *terrain[EDITOR_SIDEBAR_TERRAIN_WIDGETS];
		UI_WIDGET *buildings[EDITOR_SIDEBAR_BUILDINGS_WIDGETS];
		UI_WIDGET *properties[EDITOR_SIDEBAR_PROPERTIES_WIDGETS];
	} sidebar;
	struct {
		UI_PANE *pane;
		UI_WIDGET *palette;
	} palette;
} editor;

void editor_init();

void editor_floodfill(DARNIT_TILEMAP *tilemap, int x, int y, unsigned int tile);
void editor_reload();
void editor_palette_update(DARNIT_TILESHEET *ts);
void editor_minimap_update();
void editor_minimap_update_viewport();

void editor_topbar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void editor_sidebar_minimap_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void editor_sidebar_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void editor_sidebar_terrain_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void editor_sidebar_buildings_listbox_player_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void editor_sidebar_buildings_listbox_building_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void editor_palette_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void editor_mouse_move(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) ;
void editor_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

void editor_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void editor_palette_render(UI_WIDGET *widget);
void editor_minimap_render(UI_WIDGET *widget);
void editor_render();

#endif
