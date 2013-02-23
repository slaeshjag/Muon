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

#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))

enum MINIMAP_COLOR {
	MINIMAP_COLOR_FOW,
	MINIMAP_COLOR_TERRAIN,
	MINIMAP_COLOR_CONTROLPOINT,
	MINIMAP_COLOR_PLASMA,
};

static const unsigned int minimap_color[]={
	0xFF7F7F7F,
	0xFF165A73,
	0xFFFFFF,
	0xFF000030,
};

struct COLOR {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

static const struct COLOR player_color[]={
	{255, 0, 0},
	{0, 0, 255},
	{0, 255, 0},
	{255, 255, 0},
	
	{255, 0, 255},
	{0, 255, 255},
	{127, 127, 127},
	{191, 127, 31},
};

static const char *topbar_button_text[EDITOR_TOPBAR_BUTTONS]={
	"Menu",
	"Mirror X",
	"Mirror Y",
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

static int building_place=-1;
enum TERRAIN_TOOL terrain_tool=TERRAIN_TOOL_NONE;
static int terrain_tile=0;

struct {
	int x1;
	int y1;
	int x2;
	int y2;
} terrain_rectange_coords;

DARNIT_RECT *terrain_rectangle;

DARNIT_MAP *terrain_palette=NULL;
DARNIT_LINE *terrain_palette_selected;

DARNIT_LINE *map_border;

unsigned int minimap_data[(SIDEBAR_WIDTH-UI_PADDING*2)*(SIDEBAR_WIDTH-UI_PADDING*2)];
DARNIT_LINE *minimap_viewport;

void editor_init() {
	int i;
	editor.topbar.pane=ui_pane_create(0, 0, platform.screen_w, 32, ui_widget_create_hbox());
	//editor.topbar.pane->background_color.r=editor.topbar.pane->background_color.g=editor.topbar.pane->background_color.b=0xCD;
	
	editor.topbar.button[EDITOR_TOPBAR_BUTTON_MENU]=ui_widget_create_button_text(font_std, topbar_button_text[EDITOR_TOPBAR_BUTTON_MENU]);
	editor.topbar.button[EDITOR_TOPBAR_BUTTON_MENU]->event_handler->add(editor.topbar.button[EDITOR_TOPBAR_BUTTON_MENU], editor_topbar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_hbox_add_child(editor.topbar.pane->root_widget, editor.topbar.button[EDITOR_TOPBAR_BUTTON_MENU], 0);
	ui_hbox_add_child(editor.topbar.pane->root_widget, ui_widget_create_spacer(), 1);
	
	ui_hbox_add_child(editor.topbar.pane->root_widget, editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORX]=ui_widget_create_checkbox(), 0);
	ui_hbox_add_child(editor.topbar.pane->root_widget, ui_widget_create_label(font_std, topbar_button_text[EDITOR_TOPBAR_CHECKBOX_MIRRORX]), 0);
	ui_hbox_add_child(editor.topbar.pane->root_widget, editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORY]=ui_widget_create_checkbox(), 0);
	ui_hbox_add_child(editor.topbar.pane->root_widget, ui_widget_create_label(font_std, topbar_button_text[EDITOR_TOPBAR_CHECKBOX_MIRRORY]), 0);
	
	for(i=EDITOR_TOPBAR_BUTTON_TERRAIN; i<EDITOR_TOPBAR_BUTTONS; i++) {
		editor.topbar.button[i]=ui_widget_create_button_text(font_std, topbar_button_text[i]);
		editor.topbar.button[i]->event_handler->add(editor.topbar.button[i], editor_topbar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		ui_hbox_add_child(editor.topbar.pane->root_widget, editor.topbar.button[i], 0);
	}
	
	editor.sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 32, SIDEBAR_WIDTH, platform.screen_h-32, ui_widget_create_vbox());
	//editor.sidebar.pane->background_color.r=editor.sidebar.pane->background_color.g=editor.sidebar.pane->background_color.b=0xCD;
	
	editor.sidebar.minimap=ui_widget_create_imageview_raw(SIDEBAR_WIDTH-8, SIDEBAR_WIDTH-8, DARNIT_PFORMAT_RGB5A1);
	editor.sidebar.minimap->render=editor_minimap_render;
	editor.sidebar.minimap->event_handler->add(editor.sidebar.minimap, editor_sidebar_minimap_mouse_down, UI_EVENT_TYPE_MOUSE_DOWN);
	
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
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_BUCKET]=ui_widget_create_button_text(font_std, "Bucket");
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_RECTANGLE]=ui_widget_create_button_text(font_std, "Rectangle");
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_SPACER]=ui_widget_create_spacer_size(1, 16);
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_PALETTE]=ui_widget_create_button_text(font_std, "Tile palette");
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_ERASER]=ui_widget_create_button_text(font_std, "Tile 0 (erase)");
	
	for(i=EDITOR_SIDEBAR_TERRAIN_BUTTON_BRUSH; i<=EDITOR_SIDEBAR_TERRAIN_BUTTON_RECTANGLE; i++)
		editor.sidebar.terrain[i]->event_handler->add(editor.sidebar.terrain[i], editor_sidebar_terrain_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_PALETTE]->event_handler->add(editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_PALETTE], editor_sidebar_terrain_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_ERASER]->event_handler->add(editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_ERASER], editor_sidebar_terrain_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
		
	terrain_rectangle=d_render_rect_new(1);
	d_render_rect_move(terrain_rectangle, 0, 0, 0, 0, 0);
	terrain_palette_selected=d_render_line_new(4, 1);
	for(i=0; i<4; i++)
		d_render_line_move(terrain_palette_selected, i, 0, 0, 0, 0);
	
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
	
	ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.minimap, 0);
	for(i=0; i<EDITOR_SIDEBAR_PROPERTIES_WIDGETS; i++)
		ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.properties[i], 0);
	
	editor.palette.palette=ui_widget_create_spacer();
	editor.palette.palette->event_handler->add(editor.palette.palette, editor_palette_click, UI_EVENT_TYPE_MOUSE_PRESS);
	editor.palette.palette->render=editor_palette_render;
	editor.palette.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH-128, platform.screen_h/2-64, 128, 128, editor.palette.palette);
	
	map_border=d_render_line_new(4, 1);
	for(i=0; i<4; i++)
		d_render_line_move(map_border, i, 0, 0, 0, 0);
	
	minimap_viewport=d_render_line_new(4, 1);
	for(i=0; i<4; i++)
		d_render_line_move(minimap_viewport, i, 0, 0, 0, 0);
	for(i=0; i<(SIDEBAR_WIDTH-8)*(SIDEBAR_WIDTH-8); i++)
		((unsigned int *)minimap_data)[i]=0;
	
	state[STATE_EDITOR].panelist=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_EDITOR].panelist->next=malloc(sizeof(struct UI_PANE_LIST));
	state[STATE_EDITOR].panelist->pane=editor.topbar.pane;
	state[STATE_EDITOR].panelist->next->pane=editor.sidebar.pane;
	state[STATE_EDITOR].panelist->next->next=NULL;
	state[STATE_EDITOR].render=editor_render;
}

/*Editor functions*/

void editor_floodfill(DARNIT_TILEMAP *tilemap, int x, int y, unsigned int tile) {
	int i;
	tilemap->data[y*tilemap->w+x]=tile;
	
	map->map->layer[map->map->layers-2].tilemap->data[y*map->map->layer[map->map->layers-2].tilemap->w+x]|=(1<<17);
	for(i=0; i<map->map->layers-2; i++)
		if(map->map->layer[i].tilemap->data[y*map->map->layer[i].tilemap->w+x]) {
			map->map->layer[map->map->layers-2].tilemap->data[y*map->map->layer[map->map->layers-2].tilemap->w+x]&=~(1<<17);
			break;
		}
	
	if(x>0&&tilemap->data[y*tilemap->w+(x-1)]!=tile)
		editor_floodfill(tilemap, x-1, y, tile);
	if(x<tilemap->w-1&&tilemap->data[y*tilemap->w+(x+1)]!=tile)
		editor_floodfill(tilemap, x+1, y, tile);
	if(y>0&&tilemap->data[(y-1)*tilemap->w+x]!=tile)
		editor_floodfill(tilemap, x, y-1, tile);
	if(y<tilemap->h-1&&tilemap->data[(y+1)*tilemap->w+x]!=tile)
		editor_floodfill(tilemap, x, y+1, tile);
}

void editor_reload() {
	editor_palette_update(map->map->layer->ts);
	d_render_line_move(map_border, 0, 0, 0, map->w, 0);
	d_render_line_move(map_border, 1, 0, map->h, map->w, map->h);
	d_render_line_move(map_border, 2, 0, 0, 0, map->h);
	d_render_line_move(map_border, 3, map->w, 0, map->w, map->h);
	editor_minimap_update();
	editor_minimap_update_viewport();
}

void editor_palette_update(DARNIT_TILESHEET *ts) {
	int i, w, h, hh, tile_w, tile_h;
	d_render_tilesheet_geometrics(ts, &w, &h, &tile_w, &tile_h);
	hh=h-tile_h*9;
	ui_pane_resize(editor.palette.pane, platform.screen_w-SIDEBAR_WIDTH-w-UI_PADDING*2, platform.screen_h/2-hh/2, w+UI_PADDING*2, hh+UI_PADDING*2);
	terrain_palette=map_new_palette(w/tile_w, hh/tile_h, ts);
	for(i=0; i<w/tile_w*((h)/tile_h-9); i++)
		terrain_palette->layer->tilemap->data[i]=i+(w/tile_w)*9;
	
	d_map_camera_move(terrain_palette, -editor.palette.palette->x, -editor.palette.palette->y);
	d_tilemap_recalc(terrain_palette->layer->tilemap);
}

void editor_minimap_update() {
	//This is really slow and retarded, but it works
	int x, y, w, h;
	int tilesx;
	DARNIT_TILEMAP *building_tilemap=map->map->layer[map->map->layers-2].tilemap;
	DARNIT_TILESHEET *ts=(editor.sidebar.minimap->get_prop(editor.sidebar.minimap, UI_IMAGEVIEW_PROP_TILESHEET)).p;
	w=editor.sidebar.minimap->w;
	h=editor.sidebar.minimap->h;
	
	d_render_tilesheet_geometrics(map->map->layer->ts, &tilesx, NULL, NULL, NULL);
	tilesx/=map->map->layer->tile_w;
	
	for(y=0; y<h; y++)
		for(x=0; x<h; x++) {
			int index=(y*(building_tilemap->h)/(h))*(building_tilemap->w)+(x*(building_tilemap->w))/(w);
			minimap_data[y*w+x]=minimap_color[(((building_tilemap->data[index]&(1<<17))==0)?MINIMAP_COLOR_TERRAIN:MINIMAP_COLOR_PLASMA)];
		}
	
	for(y=0; y<building_tilemap->h; y++)
		for(x=0; x<building_tilemap->w; x++)
			if((building_tilemap->data[(y*building_tilemap->w)+x]&0xFFF)==5)
				minimap_data[(y*(h))/(building_tilemap->h)*(w)+(x*(w))/(building_tilemap->w)]=minimap_color[MINIMAP_COLOR_CONTROLPOINT];
			else if((building_tilemap->data[(y*building_tilemap->w)+x]&0xFFF)>0) {
				int i=((building_tilemap->data[(y*building_tilemap->w)+x])&0xFFF)/tilesx-1;
				unsigned int c=player_color[i].r|(player_color[i].g<<8)|(player_color[i].b<<16);
				minimap_data[(y*(h))/(building_tilemap->h)*(w)+(x*(w))/(building_tilemap->w)]=c;
			}
	
	d_render_tilesheet_update(ts, 0, 0, w, h, minimap_data);
}

/*Editor events*/

void editor_topbar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	UI_PROPERTY_VALUE v;
	v.i=building_place=terrain_tool=-1;
	editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]->set_prop(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], UI_LISTBOX_PROP_SELECTED, v);
	ui_widget_destroy(editor.sidebar.pane->root_widget);
	ui_pane_set_root_widget(editor.sidebar.pane, ui_widget_create_vbox());
	ui_vbox_add_child(editor.sidebar.pane->root_widget, editor.sidebar.minimap, 0);
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

void editor_minimap_update_viewport() {
	int x=editor.sidebar.minimap->w*map->map->cam_x/map->w+editor.sidebar.minimap->x;
	int y=editor.sidebar.minimap->h*(map->map->cam_y+32)/map->h+editor.sidebar.minimap->y;
	int w=editor.sidebar.minimap->w*(platform.screen_w-SIDEBAR_WIDTH)/map->w;
	int h=editor.sidebar.minimap->h*(platform.screen_h-32)/map->h;
	int x1=MAX(x, editor.sidebar.minimap->x);
	int x2=MIN(x+w, editor.sidebar.minimap->x+editor.sidebar.minimap->w);
	int y1=MAX(y, editor.sidebar.minimap->y);
	int y2=MIN(y+h, editor.sidebar.minimap->y+editor.sidebar.minimap->h);
	d_render_line_move(minimap_viewport, 0, x1, y1, x2, y1);
	d_render_line_move(minimap_viewport, 1, x1, y2, x2, y2);
	d_render_line_move(minimap_viewport, 2, x1, y1, x1, y2);
	d_render_line_move(minimap_viewport, 3, x2, y1, x2, y2);
}

void editor_sidebar_minimap_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int x=(map->w*(e->mouse->x-widget->x)/widget->w);
	int y=(map->h*(e->mouse->y-widget->y)/widget->h);
	x-=(platform.screen_w-SIDEBAR_WIDTH)/2;
	y-=(platform.screen_h-32)/2;
	
	d_map_camera_move(map->map, x, y+16);
	editor_minimap_update_viewport();
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
	int i;
	if(widget==editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_PALETTE]) {
		if(state[STATE_EDITOR].panelist->next->next) {
			free(state[STATE_EDITOR].panelist->next->next);
			state[STATE_EDITOR].panelist->next->next=NULL;
		} else {
			state[STATE_EDITOR].panelist->next->next=malloc(sizeof(struct UI_PANE_LIST));
			state[STATE_EDITOR].panelist->next->next->pane=editor.palette.pane;
			state[STATE_EDITOR].panelist->next->next->next=NULL;
		}
		return;
	} else if(widget==editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_BUTTON_ERASER]) {
		terrain_tile=0;
		for(i=0; i<4; i++)
			d_render_line_move(terrain_palette_selected, i, 0, 0, 0, 0);
		return;
	}
	
	for(i=EDITOR_SIDEBAR_TERRAIN_BUTTON_BRUSH; widget!=editor.sidebar.terrain[i]; i++);
	terrain_tool=i-EDITOR_SIDEBAR_TERRAIN_BUTTON_BRUSH;
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

void editor_palette_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int x, y;
	x=(e->mouse->x-widget->x)/terrain_palette->layer->tile_w;
	y=(e->mouse->y-widget->y)/terrain_palette->layer->tile_h;
	terrain_tile=(y+9)*terrain_palette->layer->tilemap->w+x;
	d_render_line_move(terrain_palette_selected, 0,
		widget->x+x*terrain_palette->layer->tile_w,
		widget->y+y*terrain_palette->layer->tile_h,
		widget->x+x*terrain_palette->layer->tile_w,
		widget->y+(y+1)*terrain_palette->layer->tile_h
	);
	d_render_line_move(terrain_palette_selected, 1,
		widget->x+(x+1)*terrain_palette->layer->tile_w,
		widget->y+y*terrain_palette->layer->tile_h,
		widget->x+(x+1)*terrain_palette->layer->tile_w,
		widget->y+(y+1)*terrain_palette->layer->tile_h
	);
	d_render_line_move(terrain_palette_selected, 2,
		widget->x+x*terrain_palette->layer->tile_w,
		widget->y+y*terrain_palette->layer->tile_h,
		widget->x+(x+1)*terrain_palette->layer->tile_w,
		widget->y+y*terrain_palette->layer->tile_h
	);
	d_render_line_move(terrain_palette_selected, 3,
		widget->x+x*terrain_palette->layer->tile_w,
		widget->y+(y+1)*terrain_palette->layer->tile_h,
		widget->x+(x+1)*terrain_palette->layer->tile_w,
		widget->y+(y+1)*terrain_palette->layer->tile_h
	);
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
	editor_minimap_update_viewport();
}

void editor_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	//Make sure there is no pane in the way
	if(e->mouse->x>platform.screen_w-SIDEBAR_WIDTH||e->mouse->y<32)
		return;
	if(state[STATE_EDITOR].panelist->next->next&&PINR(e->mouse->x, e->mouse->y, editor.palette.pane->x, editor.palette.pane->y, editor.palette.pane->w, editor.palette.pane->h))
		return;
	
	if(type==UI_EVENT_TYPE_MOUSE_RELEASE) {
		if(!(e->mouse->buttons&~UI_EVENT_MOUSE_BUTTON_LEFT)&&terrain_tool==TERRAIN_TOOL_RECTANGLE) {
			int x, y, layer;
			DARNIT_TILEMAP *tilemap;
			v=editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS]->get_prop(editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS], UI_LISTBOX_PROP_SELECTED);
			layer=v.i>0&&v.i<map->map->layers?v.i:0;
			tilemap=map->map->layer[layer].tilemap;
			for(y=MIN(terrain_rectange_coords.y1, terrain_rectange_coords.y2); y<MAX(terrain_rectange_coords.y1, terrain_rectange_coords.y2); y++)
				for(x=MIN(terrain_rectange_coords.x1, terrain_rectange_coords.x2); x<MAX(terrain_rectange_coords.x1, terrain_rectange_coords.x2); x++)
					map_tile_set(map->map->layer[layer].tilemap, x ,y,
						(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORX]->get_prop(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORX], UI_CHECKBOX_PROP_ACTIVATED)).i,
						(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORY]->get_prop(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORY], UI_CHECKBOX_PROP_ACTIVATED)).i,
						terrain_tile
					);
			
			d_tilemap_recalc(tilemap);
			terrain_rectange_coords.x1=terrain_rectange_coords.y1=terrain_rectange_coords.x2=terrain_rectange_coords.y2=0;
			d_render_rect_move(terrain_rectangle, 0, 0, 0, 0, 0);
		}
		editor_minimap_update();
		return;
	}
	
	if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_RIGHT&&type==UI_EVENT_TYPE_MOUSE_PRESS) {
		v.i=building_place=-1;
		editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING]->set_prop(editor.sidebar.buildings[EDITOR_SIDEBAR_BUILDINGS_LISTBOX_BUILDING], UI_LISTBOX_PROP_SELECTED, v);	
		terrain_tool=TERRAIN_TOOL_NONE;
	} else if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_LEFT) {
		int map_offset=((e->mouse->y+map->map->cam_y)/map->map->layer->tile_h)*map->map->layer->tilemap->w+((e->mouse->x+map->map->cam_x)/map->map->layer->tile_w)%map->map->layer->tilemap->w;
		if(map_offset<0||map_offset>map->map->layer->tilemap->w*map->map->layer->tilemap->h)
			return;
		if(type==UI_EVENT_TYPE_MOUSE_PRESS&&building_place>-1&&map->map->layer[map->map->layers-2].tilemap->data[map_offset]!=building_place) {
			//map->map->layer[map->map->layers-2].tilemap->data[map_offset]=building_place;
			int x=((e->mouse->x+map->map->cam_x)/map->map->layer[map->map->layers-2].tile_w);
			int y=((e->mouse->y+map->map->cam_y)/map->map->layer[map->map->layers-2].tile_h);
			map_tile_set(map->map->layer[map->map->layers-2].tilemap, x, y,
				(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORX]->get_prop(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORX], UI_CHECKBOX_PROP_ACTIVATED)).i,
				(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORY]->get_prop(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORY], UI_CHECKBOX_PROP_ACTIVATED)).i,
				building_place
			);
			d_tilemap_recalc(map->map->layer[map->map->layers-2].tilemap);
		} else if(terrain_tool>TERRAIN_TOOL_NONE) {
			int layer;
			int x=((e->mouse->x+map->map->cam_x)/map->map->layer->tile_w);
			int y=((e->mouse->y+map->map->cam_y)/map->map->layer->tile_h);
			v=editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS]->get_prop(editor.sidebar.terrain[EDITOR_SIDEBAR_TERRAIN_LISTBOX_LAYERS], UI_LISTBOX_PROP_SELECTED);
			layer=v.i>0&&v.i<map->map->layers?v.i:0;
			switch(terrain_tool) {
				case TERRAIN_TOOL_BRUSH:
					map_tile_set(map->map->layer[layer].tilemap, x ,y,
						(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORX]->get_prop(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORX], UI_CHECKBOX_PROP_ACTIVATED)).i,
						(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORY]->get_prop(editor.topbar.button[EDITOR_TOPBAR_CHECKBOX_MIRRORY], UI_CHECKBOX_PROP_ACTIVATED)).i,
						terrain_tile
					);
					d_tilemap_recalc(map->map->layer[layer].tilemap);
					break;
				case TERRAIN_TOOL_BUCKET:
					if(type==UI_EVENT_TYPE_MOUSE_PRESS) {
						if(map->map->layer[layer].tilemap->data[map_offset]==terrain_tile)
							break;
						editor_floodfill(map->map->layer[layer].tilemap, x, y, terrain_tile);
						d_tilemap_recalc(map->map->layer[layer].tilemap);
						editor_minimap_update();
					}
					break;
				case TERRAIN_TOOL_RECTANGLE:
						if(type==UI_EVENT_TYPE_MOUSE_PRESS) {
							terrain_rectange_coords.x1=x;
							terrain_rectange_coords.y1=y;
							break;
						}
						terrain_rectange_coords.x2=x<0?1:x+1>map->map->layer[layer].tilemap->w?map->map->layer[layer].tilemap->w:x+1;
						terrain_rectange_coords.y2=y<0?1:y+1>map->map->layer[layer].tilemap->h?map->map->layer[layer].tilemap->h:y+1;
						d_render_rect_move(terrain_rectangle, 0,
							terrain_rectange_coords.x1*map->map->layer[layer].tile_w,
							terrain_rectange_coords.y1*map->map->layer[layer].tile_h,
							terrain_rectange_coords.x2*map->map->layer[layer].tile_w,
							terrain_rectange_coords.y2*map->map->layer[layer].tile_h
						);
					break;
				default:
					break;
			}
		}
	}
}

/*Render functions*/

void editor_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	editor_mouse_move(widget, type, e);
	if(building_place>-1&&e->mouse->x<platform.screen_w-SIDEBAR_WIDTH) {
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

void editor_palette_render(UI_WIDGET *widget) {
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(255, 255, 255, 255);
	d_tilemap_draw(terrain_palette->layer->tilemap);
	if(terrain_tile>=terrain_palette->layer->tilemap->w*9)
		d_render_line_draw(terrain_palette_selected, 4);
	d_render_tint(r, g, b, a);
}

void editor_minimap_render(UI_WIDGET *widget) {
	//override for imagebox render, to render viewport border on minimap
	unsigned char r, g, b, a;
	ui_imageview_render(widget);
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(255, 255, 255, 255);
	d_render_line_draw(minimap_viewport, 4);
	d_render_tint(r, g, b, a);
}

void editor_render() {
	int i;
	for(i=0; i<map->map->layers-1; i++)
		d_tilemap_draw(map->map->layer[i].tilemap);
	
	d_render_offset(map->map->cam_x, map->map->cam_y);
	d_render_line_draw(map_border, 4);
	
	if(terrain_tool==TERRAIN_TOOL_RECTANGLE) {
		d_render_tint(255, 255, 255, 127);
		d_render_blend_enable();
		d_render_offset(map->map->cam_x, map->map->cam_y);
		d_render_rect_draw(terrain_rectangle, 1);
		d_render_blend_disable();
		d_render_tint(255, 255, 255, 255);
	}
	d_render_offset(0, 0);
}
