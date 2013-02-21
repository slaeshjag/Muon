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

#include <string.h>

#include "maped.h"
#include "map.h"
#include "editor.h"

static const char *topbar_button_text[EDITOR_TOPBAR_BUTTONS]={
	"Menu",
	"Terrain",
	"Buildings",
	"Properties"
};

static const char *player_text[MAX_PLAYERS+1]={
	"[Neutral]",
	"Player 0",
	"Player 1",
	"Player 2",
	"Player 3",
};

static const char *building_text[]={
	"Generator",
	"Scout",
	"Attacker",
	"Pylon",
	"Wall",
	"Conyard",
	"Missile silo",
	"Radar",
};

enum TERRAIN_TOOL {
	TERRAIN_TOOL_NONE=-1,
	TERRAIN_TOOL_BRUSH,
	TERRAIN_TOOL_BUCKET,
	TERRAIN_TOOL_RECTANGLE,
	TERRAIN_TOOLS,
};

static int building_place=-1;
static enum TERRAIN_TOOL terrain_tool=TERRAIN_TOOL_NONE;
static int terrain_tile=0;

void editor_init() {
	int i;
	editor.topbar.pane=ui_pane_create(0, 0, platform.screen_w, 32, ui_widget_create_hbox());
	//editor.topbar.pane->background_color.r=editor.topbar.pane->background_color.g=editor.topbar.pane->background_color.b=0xCD;

	for(i=0; i<EDITOR_TOPBAR_BUTTONS; i++) {
		editor.topbar.button[i]=ui_widget_create_button_text(font_std, topbar_button_text[i]);
		editor.topbar.button[i]->event_handler->add(editor.topbar.button[i], editor_topbar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		ui_vbox_add_child(editor.topbar.pane->root_widget, editor.topbar.button[i], 0);
		if(i==0)
			ui_vbox_add_child(editor.topbar.pane->root_widget, ui_widget_create_spacer(), 1);
	}
	
	editor.sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 32, SIDEBAR_WIDTH, platform.screen_h-32, ui_widget_create_vbox());
	//editor.sidebar.pane->background_color.r=editor.sidebar.pane->background_color.g=editor.sidebar.pane->background_color.b=0xCD;
	
	/*Menu tab*/
	editor.sidebar.menu[EDITOR_SIDEBAR_MENU_LABEL]=ui_widget_create_label(font_std, "Menu");
	editor.sidebar.menu[EDITOR_SIDEBAR_MENU_BUTTON_SAVE]=ui_widget_create_button_text(font_std, "Save");
	editor.sidebar.menu[EDITOR_SIDEBAR_MENU_BUTTON_QUIT]=ui_widget_create_button_text(font_std, "Quit");
	for(i=EDITOR_SIDEBAR_MENU_BUTTON_SAVE; i<EDITOR_SIDEBAR_MENU_WIDGETS; i++)
		editor.sidebar.menu[i]->event_handler->add(editor.sidebar.menu[i], editor_sidebar_menu_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	
	/*Terrain tab*/
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LABEL]=ui_widget_create_label(font_std, "Terrain");
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LABEL_LAYERS]=ui_widget_create_label(font_std, "Layers");
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS]=ui_widget_create_listbox(font_std);
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_BRUSH]=ui_widget_create_button_text(font_std, "Brush");
	for(i=EDITOR_SIDEBAR_TERRAIN_BUTTON_BRUSH; i<EDITOR_SIDEBAR_TERRAIN_WIDGETS; i++)
		editor.sidebar.terrain[i]->event_handler->add(editor.sidebar.terrain[i], editor_sidebar_terrain_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	
	/*Buildings tab*/
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LABEL]=ui_widget_create_label(font_std, "Buildings");
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LABEL_PLAYER]=ui_widget_create_label(font_std, "Player");
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER]=ui_widget_create_listbox(font_std);
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LABEL_BUILDING]=ui_widget_create_label(font_std, "Building");
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]=ui_widget_create_listbox(font_std);
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_SPACER]=ui_widget_create_spacer_size(1, 4);
	for(i=0; i<MAX_PLAYERS+1; i++)
		ui_listbox_add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER], player_text[i]);
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER]->event_handler->add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER], editor_sidebar_buildings_listbox_player_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]->event_handler->add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], editor_sidebar_buildings_listbox_building_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	
	/*Properties tab*/
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_LABEL]=ui_widget_create_label(font_std, "Properites");
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_LABEL_NAME]=ui_widget_create_label(font_std, "Map name");
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_NAME]=ui_widget_create_entry(font_std);
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_LABEL_VERSION]=ui_widget_create_label(font_std, "Version");
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_VERSION]=ui_widget_create_entry(font_std);
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_LABEL_AUTHOR]=ui_widget_create_label(font_std, "Map author");
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_AUTHOR]=ui_widget_create_entry(font_std);
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_LABEL_PLAYERS]=ui_widget_create_label(font_std, "Max players");
	editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_SLIDER_PLAYERS]=ui_widget_create_slider(4);
	
	for(i=0; i<EDITOR_SIDEBAR_PROPERTIES_WIDGETS; i++)
			ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.properties[i], 0);
	
	state[STATE_EDITOR].panelist=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_EDITOR].panelist->next=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_EDITOR].panelist->pane=editor.topbar.pane;
	state[STATE_EDITOR].panelist->next->pane=editor.sidebar.pane;
	state[STATE_EDITOR].panelist->next->next=NULL;
	state[STATE_EDITOR].render=editor_render;
}

void editor_topbar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	UI_PROPERTY_VALUE v;
	v.i=building_place=terrain_tool=-1;
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]->set_prop(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], UI_LISTBOX_PROP_SELECTED, v);
	ui_widget_destroy(editor.sidebar.pane->root_widget);
	ui_pane_set_root_widget(editor.sidebar.pane, ui_widget_create_vbox());
	if(widget==editor.topbar.button[EDITOR_TOPBAR_BUTTON_MENU]) {
		for(i=0; i<EDITOR_SIDEBAR_MENU_WIDGETS; i++)
			ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.menu[i], 0);
	} else if(widget==editor.topbar.button[EDITOR_TOPBAR_BUTTON_TERRAIN]) {
		for(i=0; i<EDITOR_SIDEBAR_TERRAIN_WIDGETS; i++)
			ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.terrain[i], 0);
	} else if(widget==editor.topbar.button[EDITOR_TOPBAR_BUTTON_BUILDINGS]) {
		for(i=0; i<EDITOR_SIDEBAR_BUILDINGS_WIDGETS; i++)
			ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.buildings[i], i&&!(i&1));
	} else if(widget==editor.topbar.button[EDITOR_TOPBAR_BUTTON_PROPERTIES]) {
		for(i=0; i<EDITOR_SIDEBAR_PROPERTIES_WIDGETS; i++)
			ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.properties[i], 0);
	}
}

void editor_sidebar_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	UI_PROPERTY_VALUE v;
	static char buf_players[2];
	char filename[128], *name, *version;
	for(i=EDITOR_SIDEBAR_MENU_BUTTON_SAVE; widget!=editor.sidebar.menu[i]; i++);
	switch(i) {
		case EDITOR_SIDEBAR_MENU_BUTTON_SAVE:
			v=editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_NAME]->get_prop(editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_NAME], UI_ENTRY_PROP_TEXT);
			if(!strlen(name=v.p)) {
				ui_messagebox(font_std, "Map is missing name property");
				break;
			}
			map_prop_set_or_add(map, "name", name);
			v=editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_VERSION]->get_prop(editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_VERSION], UI_ENTRY_PROP_TEXT);
			if(!strlen(version=v.p)) {
				ui_messagebox(font_std, "Map is missing version property");
				break;
			}
			map_prop_set_or_add(map, "version", version);
			v=editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_AUTHOR]->get_prop(editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_ENTRY_AUTHOR], UI_ENTRY_PROP_TEXT);
			if(!strlen(v.p)) {
				ui_messagebox(font_std, "Map is missing author property");
				break;
			}
			map_prop_set_or_add(map, "author", v.p);
			v=editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_SLIDER_PLAYERS]->get_prop(editor.sidebar.properties[EDITOR_SIDEBAR_PROPERTIES_SLIDER_PLAYERS], UI_SLIDER_PROP_VALUE);
			sprintf(buf_players, "%i", v.i+1);
			map_prop_set_or_add(map, "max_players", buf_players);
			sprintf(filename, "maps/%s %s (%s).ldi", name, version, buf_players);
			map_save(map, filename);
			break;
		case EDITOR_SIDEBAR_MENU_BUTTON_QUIT:
			d_quit();
			break;
	}
}

void editor_sidebar_terrain_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	terrain_tool=TERRAIN_TOOL_BRUSH;
}

void editor_sidebar_buildings_listbox_player_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	UI_PROPERTY_VALUE v;
	building_place=-1;
	v=widget->get_prop(widget, UI_LISTBOX_PROP_SELECTED);
	if(v.i==-1)
		return;
	ui_listbox_clear(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]);
	if(v.i==0) {
		ui_listbox_add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], "[Erase]");
		ui_listbox_add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], "Control point");
	} else
		for(i=0; i<BUILDINGS; i++)
			ui_listbox_add(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], building_text[i]);
}

void editor_sidebar_buildings_listbox_building_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int building;
	UI_PROPERTY_VALUE v;
	v=widget->get_prop(widget, UI_LISTBOX_PROP_SELECTED);
	building=v.i;
	v=editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER]->get_prop(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_PLAYER], UI_LISTBOX_PROP_SELECTED);
	
	building_place=building&&!v.i?5:v.i*16+building;
}

void editor_mouse_move(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int scroll_x=0, scroll_y=0;
	int screen_w=platform.screen_w, screen_h=platform.screen_h;
	
	if(e->mouse->x<SCROLL_OFFSET&&map->map->cam_x>-((screen_w-SIDEBAR_WIDTH)/2))
		scroll_x=-SCROLL_SPEED;
	else if(e->mouse->x>platform.screen_w-SCROLL_OFFSET&&map->map->cam_x<map->w-(screen_w-SIDEBAR_WIDTH)/2)
		scroll_x=SCROLL_SPEED;
	if(e->mouse->y<SCROLL_OFFSET&&map->map->cam_y>-(screen_h)/2)
		scroll_y=-SCROLL_SPEED;
	else if(e->mouse->y>platform.screen_h-SCROLL_OFFSET&&map->map->cam_y<map->h-screen_h/2)
		scroll_y=SCROLL_SPEED;
		
	if(!scroll_x&&!scroll_y)
		return;
	
	d_map_camera_move(map->map, map->map->cam_x+scroll_x, map->map->cam_y+scroll_y);
	/*map_minimap_update_viewport();*/
}

void editor_mouse_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	//Make sure there is no pane in the way
	if(e->mouse->x>platform.screen_w-SIDEBAR_WIDTH||e->mouse->y<32)
		return;
	
	if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_RIGHT&&type==UI_EVENT_TYPE_MOUSE_PRESS) {
		v.i=building_place=-1;
		editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]->set_prop(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], UI_LISTBOX_PROP_SELECTED, v);	
		terrain_tool=TERRAIN_TOOL_NONE;
	} else if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_LEFT) {
		int map_offset=((e->mouse->y+map->map->cam_y)/map->map->layer->tile_h)*map->map->layer->tilemap->w+((e->mouse->x+map->map->cam_x)/map->map->layer->tile_w)%map->map->layer->tilemap->w;
		if(map_offset<0||map_offset>map->map->layer->tilemap->w*map->map->layer->tilemap->h)
			return;
		if(type==UI_EVENT_TYPE_MOUSE_PRESS&&building_place>-1&&map->map->layer[map->map->layers-2].tilemap->data[map_offset]!=building_place) {
			map->map->layer[map->map->layers-2].tilemap->data[map_offset]=building_place;
			d_tilemap_recalc(map->map->layer[map->map->layers-2].tilemap);
		} else if(terrain_tool>TERRAIN_TOOL_NONE) {
			int layer;
			v=editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS]->get_prop(editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS], UI_LISTBOX_PROP_SELECTED);
			layer=v.i>0&&v.i<map->map->layers?v.i:0;
			switch(terrain_tool) {
				case TERRAIN_TOOL_BRUSH:
					if(map->map->layer[layer].tilemap->data[map_offset]!=terrain_tile) {
						map->map->layer[layer].tilemap->data[map_offset]=terrain_tile;
						d_tilemap_recalc(map->map->layer[layer].tilemap);
					}
					break;
				default:
					break;
			}
		}
	}
}

/*void editor_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	if(e->mouse->x>platform.screen_w-SIDEBAR_WIDTH||e->mouse->y<32)
		return;
	
	if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_LEFT) {
		int map_offset=((e->mouse->y+map->map->cam_y)/map->map->layer->tile_h)*map->map->layer->tilemap->w+((e->mouse->x+map->map->cam_x)/map->map->layer->tile_w)%map->map->layer->tilemap->w;
		if(map_offset<0||map_offset>map->map->layer->tilemap->w*map->map->layer->tilemap->h)
			return;
		if(terrain_tool>TERRAIN_TOOL_NONE) {
			int layer;
			v=editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS]->get_prop(editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS], UI_LISTBOX_PROP_SELECTED);
			layer=v.i>0&&v.i<map->map->layers?v.i:0;
			switch(terrain_tool) {
				case TERRAIN_TOOL_BRUSH:
					if(map->map->layer[layer].tilemap->data[map_offset]!=terrain_tile) {
						map->map->layer[layer].tilemap->data[map_offset]=terrain_tile;
						d_tilemap_recalc(map->map->layer[layer].tilemap);
					}
					break;
				default:
					break;
			}
		}
	}
}*/

void editor_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	editor_mouse_move(widget, type, e);
	if(building_place>=-1&&e->mouse->x<platform.screen_w-SIDEBAR_WIDTH) {
		unsigned char r, g, b, a;
		d_render_tint_get(&r, &g, &b, &a);
		d_render_tint(255, 255, 255, 255);
		DARNIT_MAP_LAYER *l=&map->map->layer[map->map->layers-2];
		int x=(e->mouse->x+map->map->cam_x)/l->tile_w*l->tile_w;
		int y=(e->mouse->y+map->map->cam_y)/l->tile_h*l->tile_h;
		d_render_offset(map->map->cam_x, map->map->cam_y);
		d_render_blend_enable();
		d_render_tile_blit(l->ts, building_place, x, y);
		d_render_blend_disable();
		d_render_offset(0, 0);
		d_render_tint(r, g, b, a);
	}
}

void editor_render() {
	int i;
	for(i=0; i<map->map->layers-1; i++)
		d_tilemap_draw(map->map->layer[i].tilemap);
}
