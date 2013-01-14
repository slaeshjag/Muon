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

#include "muon.h"
#include "client.h"
#include "map.h"
#include "chat.h"
#include "game.h"
#include "intmath.h"

void game_view_init() {
	building_place=-1;
	
	game_attacklist_blink_semaphore=0;
	
	panelist_game_sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, platform.screen_h, NULL);
	ui_pane_set_root_widget(panelist_game_sidebar.pane, ui_widget_create_vbox());
	panelist_game_sidebar.next=NULL;
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, T("Muon")), 0);
	game_sidebar_minimap=ui_widget_create_imageview_raw(SIDEBAR_WIDTH-8, SIDEBAR_WIDTH-8, DARNIT_PFORMAT_RGB5A1);
	game_sidebar_minimap->render=map_minimap_render;
	game_sidebar_minimap->event_handler->add(game_sidebar_minimap, game_sidebar_minimap_mouse_down, UI_EVENT_TYPE_MOUSE_DOWN);
	if(!(platform.platform&DARNIT_PLATFORM_PANDORA))
		ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_minimap, 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, T("Buildings:")), 0);
	game_sidebar_label_build[0]=ui_widget_create_label(font_std, T("Scout"));
	game_sidebar_label_build[1]=ui_widget_create_label(font_std, T("Attacker"));
	game_sidebar_label_build[2]=ui_widget_create_label(font_std, T("Pylon"));
	game_sidebar_label_build[3]=ui_widget_create_label(font_std, T("Wall"));
	game_sidebar_label_build[4]=ui_widget_create_label(font_std, T("Build site"));
	int i;
	for(i=0; i<5; i++) {
		game_sidebar_button_build[i]=ui_widget_create_button(game_sidebar_label_build[i]);
		ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_button_build[i], 0);
		game_sidebar_button_build[i]->event_handler->add(game_sidebar_button_build[i], game_sidebar_button_build_click, UI_EVENT_TYPE_UI);
	}
	game_sidebar_progress_build=ui_widget_create_progressbar(font_std);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_spacer(), 1);
	game_sidebar_progress_shield=ui_widget_create_progressbar(font_std);
	game_sidebar_progress_health=ui_widget_create_progressbar(font_std);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, T("Shield")), 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_progress_shield, 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, T("Health")), 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_progress_health, 0);
	if(platform.platform&DARNIT_PLATFORM_PANDORA)
		ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_minimap, 0);
}

void game_sidebar_minimap_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_MOUSE_DOWN)
		return;
	int x=e->mouse->x-widget->x;
	int y=e->mouse->y-widget->y;
	darnitMapCameraMove(map, (map_w*x/widget->w)-platform.screen_w/2, (map_h*y/widget->h)-platform.screen_h/2);
	map_minimap_update_viewport();
}

/* building_ready values:
 *  - negative if a building is in progress
 *  - positive if a building is ready
 *  - zero (BUILDING_NONE) if no building is ready */
void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	UI_PROPERTY_VALUE v;
	int i=0;
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	// Find the building number.
	for(; widget!=game_sidebar_button_build[i]; i++);
	if(!building_cancel) {
		if(building_ready==BUILDING_SCOUT+i) { // If the building is ready, place it.
			building_place=building_ready;
		} else if(building_ready==BUILDING_NONE) { // Build if no there are no buildings in progress nor other buildings ready.
			client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+i, MSG_BUILDING_START, NULL);
			v.p=game_sidebar_progress_build;
			widget->set_prop(widget, UI_BUTTON_PROP_CHILD, v);
			game_set_building_progress(0, 0);
			game_set_building_ready(-(BUILDING_SCOUT+i));
		}
	} else if(ABS(building_ready)==BUILDING_SCOUT+i) {	// Cancel if the building is is in progress or ready and shift is pressed
		client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+i, MSG_BUILDING_STOP, NULL);
		v.p=game_sidebar_label_build[i];
		widget->set_prop(widget, UI_BUTTON_PROP_CHILD, v);
		game_set_building_progress(0, 0);
		game_set_building_ready(BUILDING_NONE); // No buildings in progress
	}
	ui_selected_widget=NULL;
}

void game_view_key_press(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	//TODO: keysyms of number keys might not be sequential on all platforms
	if(type!=UI_EVENT_TYPE_KEYBOARD_PRESS)
		return;
	if(e->keyboard->keysym>=KEY(1)&&e->keyboard->keysym<=KEY(5)) {
		game_sidebar_button_build_click(game_sidebar_button_build[e->keyboard->keysym-KEY(1)], UI_EVENT_TYPE_UI_WIDGET_ACTIVATE, NULL);
	}
}

void game_view_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int scroll_x=0, scroll_y=0;
	int screen_w=platform.screen_w, screen_h=platform.screen_h;
	//darnit buttons have no press or release events, we have to handle this ourselves
	static UI_EVENT_BUTTONS prevbuttons={0};
	
	//View movement
	if(!chat_is_visible(&panelist_game_sidebar)) {
		if(e->buttons->left&&map->cam_x>-((screen_w-SIDEBAR_WIDTH)/2))
			scroll_x=-SCROLL_SPEED;
		else if(e->buttons->right&&map->cam_x<map_w-(screen_w-SIDEBAR_WIDTH)/2)
			scroll_x=SCROLL_SPEED;
		if(e->buttons->up&&map->cam_y>-(screen_h)/2)
			scroll_y=-SCROLL_SPEED;
		else if(e->buttons->down&&map->cam_y<map_h-screen_h/2)
			scroll_y=SCROLL_SPEED;
			
		if(e->buttons->y) {
			scroll_x*=2;
			scroll_y*=2;
		}
		if(scroll_x||scroll_y) {
			darnitMapCameraMove(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
			map_minimap_update_viewport();
		}
	}
	//Interaction keys
	if(e->buttons->start&&!prevbuttons.start)
		game_state(GAME_STATE_GAME_MENU);
	if(e->buttons->x&&!prevbuttons.x)
		chat_toggle(&panelist_game_sidebar);
	if(e->buttons->b&&!prevbuttons.b) {
		if(building_place>-1) {
			building_place=-1;
		} else if(map_selected_building()) {
			client_message_send(player_id, MSG_SEND_PLACE_BUILDING, BUILDING_NONE, map_selected_index(), NULL);
			map_select_nothing();
		}
	}
	
	// If shift is pressed (button Y) set building_cancel
	building_cancel = e->buttons->y;
	
	memcpy(&prevbuttons, e->buttons, sizeof(UI_EVENT_BUTTONS));
}

void game_view_mouse_move(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int scroll_x=0, scroll_y=0;
	int screen_w=platform.screen_w, screen_h=platform.screen_h;
	
	if(e->mouse->x<SCROLL_OFFSET&&map->cam_x>-((screen_w-SIDEBAR_WIDTH)/2))
		scroll_x=-SCROLL_SPEED;
	else if(e->mouse->x>platform.screen_w-SCROLL_OFFSET&&map->cam_x<map_w-(screen_w-SIDEBAR_WIDTH)/2)
		scroll_x=SCROLL_SPEED;
	if(e->mouse->y<SCROLL_OFFSET&&map->cam_y>-(screen_h)/2)
		scroll_y=-SCROLL_SPEED;
	else if(e->mouse->y>platform.screen_h-SCROLL_OFFSET&&map->cam_y<map_h-screen_h/2)
		scroll_y=SCROLL_SPEED;
		
	if(!scroll_x&&!scroll_y)
		return;
	
	darnitMapCameraMove(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
	map_minimap_update_viewport();
}

void game_view_mouse_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(e->mouse->x>platform.screen_w-SIDEBAR_WIDTH)
		return;
	if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_RIGHT)
		building_place=-1;
	else if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_LEFT) {
		int map_offset=((e->mouse->y+map->cam_y)/map->layer->tile_h)*map->layer->tilemap->w+((e->mouse->x+map->cam_x)/map->layer->tile_w)%map->layer->tilemap->w;
		if(map_offset<0||map_offset>map->layer->tilemap->w*map->layer->tilemap->h)
			return;
		if(building_place>-1) {
			client_message_send(player_id, MSG_SEND_PLACE_BUILDING, building_place, map_offset, NULL);
			building_place=-1;
		} else {
			//status selected clicked building, etc
			map_select_building(map_offset);
			game_update_building_status();
		}
	}
}

void game_update_building_status() {
	int map_offset=map_selected.index;
	UI_PROPERTY_VALUE v;
	v.i=map_get_building_shield(map_offset);
	game_sidebar_progress_shield->set_prop(game_sidebar_progress_shield, UI_PROGRESSBAR_PROP_PROGRESS, v);
	v.i=map_get_building_health(map_offset);
	game_sidebar_progress_health->set_prop(game_sidebar_progress_health, UI_PROGRESSBAR_PROP_PROGRESS, v);
}

void game_view_scroll_to(int x, int y) {
	darnitMapCameraMove(map, x*map->layer[map->layers-2].tile_w-(platform.screen_w-SIDEBAR_WIDTH)/2, y*map->layer[map->layers-2].tile_h-platform.screen_h/2);
	map_minimap_update_viewport();
}

void game_set_building_progress(int building, int progress) {
	UI_PROPERTY_VALUE v;
	v.i=progress;
	game_sidebar_progress_build->set_prop(game_sidebar_progress_build, UI_PROGRESSBAR_PROP_PROGRESS, v);
}

void game_reset_building_progress() {
	int i;
	for(i=0; i<5; i++) {
		UI_PROPERTY_VALUE v={.p=game_sidebar_label_build[i]};
		game_sidebar_button_build[i]->set_prop(game_sidebar_button_build[i], UI_BUTTON_PROP_CHILD, v);
	}
}

void game_set_building_ready(int building) {
	building_ready = building;
}

void game_attacklist_lines_recalculate() {
	struct GAME_ATTACKLIST *l;
	int i, x1, x2, y1, y2;
	int map_wt=map->layer[map->layers-2].tilemap->w;
	//int map_ht=map->layer[map->layers-2].tilemap->h;
	int tw=map->layer[map->layers-2].tile_w;
	int th=map->layer[map->layers-2].tile_h;
	darnitRenderLineFree(game_attacklist_lines);
	game_attacklist_lines=darnitRenderLineAlloc(game_attacklist_length, 1);
	
	for(l=game_attacklist, i=0; l; l=l->next, i++) {
		x1=l->index%map_wt*tw+tw/2;
		y1=l->index/map_wt*th+th/2;
		x2=l->target%map_wt*tw+tw/2;
		y2=l->target/map_wt*th+th/2;
		darnitRenderLineMove(game_attacklist_lines, i, x1, y1, x2, y2);
	}
}

void game_attacklist_add(int index) {
	struct GAME_ATTACKLIST **l, *ll;
	for(l=&(game_attacklist); *l; l=&((*l)->next));
	if((*l=malloc(sizeof(struct GAME_ATTACKLIST)))==NULL)
		return;
	ll=*l;
	ll->index=index;
	ll->target=index;
	ll->next=NULL;
	game_attacklist_length++;
	game_attacklist_lines_recalculate();
}

void game_attacklist_remove(int index) {
	struct GAME_ATTACKLIST **l, *l_next;
	for(l=&(game_attacklist); *l; l=&((*l)->next)) {
		if((*l)->index==index) {
			l_next=(*l)->next;
			free(*l);
			*l=l_next;
			game_attacklist_length--;
			game_attacklist_lines_recalculate();
			break;
		}
	}
}

void game_attacklist_clear() {
	struct GAME_ATTACKLIST *l, *next;
	for(l=game_attacklist; l; l=next) {
		next=l->next;
		free(l);
	}
	game_attacklist=NULL;
	game_attacklist_length=0;
	game_attacklist_lines=darnitRenderLineFree(game_attacklist_lines);
}

void game_attacklist_untarget(int target) {
	struct GAME_ATTACKLIST *l;
	int recalc=0;
	for(l=game_attacklist; l; l=l->next) {
		if(l->target==target) {
			l->target=l->index;
			recalc=1;
		}
	}
	if(recalc)
		game_attacklist_lines_recalculate();
}

void game_attacklist_target(int index, int target) {
	struct GAME_ATTACKLIST *l;
	for(l=game_attacklist; l; l=l->next) {
		if(l->index==index) {
			l->target=target==-1?index:target;
			game_attacklist_lines_recalculate();
			break;
		}
	}
}

void game_view_draw() {
	view_background_draw();
	int selected=map_selected_building();
	if((building_place!=-1||selected==BUILDING_PYLON||selected==BUILDING_GENERATOR))
		map_draw(1);
	else
		map_draw(0);
	
	if(game_attacklist_blink_semaphore>30) {
		darnitRenderOffset(map->cam_x, map->cam_y);
		darnitRenderLineDraw(game_attacklist_lines, game_attacklist_length);
		darnitRenderOffset(0, 0);
	}
	if(game_attacklist_blink_semaphore>60)
		game_attacklist_blink_semaphore=0;
	game_attacklist_blink_semaphore++;
		
	//darnitRenderTileBlit(minimap, 0, 128, 32);
}

void game_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(building_place!=-1&&e->mouse->x<platform.screen_w-SIDEBAR_WIDTH) {
		DARNIT_MAP_LAYER *l=&map->layer[map->layers-1];
		int x=(e->mouse->x+map->cam_x)/l->tile_w*l->tile_w;
		int y=(e->mouse->y+map->cam_y)/l->tile_h*l->tile_h;
		darnitRenderOffset(map->cam_x, map->cam_y);
		darnitRenderBlendingEnable();
		darnitRenderTileBlit(l->ts, player_id*8+building_place+7, x, y);
		darnitRenderBlendingDisable();
		darnitRenderOffset(0, 0);
	}
	view_mouse_draw(widget, type, e);
}
