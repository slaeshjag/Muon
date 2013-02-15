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
	ability_place=0;
	attacker_target=0;
	
	building[0].name="";
	building[1].name=T("Generator");
	building[2].name=T("Scout");
	building[3].name=T("Attacker");
	building[4].name=T("Pylon");
	building[5].name=T("Wall");
	building[6].name=T("Buildsite");
	building[7].name=T("Missile silo");
	building[8].name=T("Radar");
	
	game_attacklist_blink_semaphore=0;
	/*Game sidebar*/
	panelist_game_sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, platform.screen_h, NULL);
	ui_pane_set_root_widget(panelist_game_sidebar.pane, ui_widget_create_vbox());
	panelist_game_sidebar.next=&panelist_game_abilitybar;
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, T("Muon")), 0);
	game_sidebar_minimap=ui_widget_create_imageview_raw(SIDEBAR_WIDTH-8, SIDEBAR_WIDTH-8, DARNIT_PFORMAT_RGB5A1);
	game_sidebar_minimap->render=map_minimap_render;
	game_sidebar_minimap->event_handler->add(game_sidebar_minimap, game_sidebar_minimap_mouse_down, UI_EVENT_TYPE_MOUSE_DOWN);
	if(!(platform.platform&DARNIT_PLATFORM_PANDORA))
		ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_minimap, 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, T("Buildings:")), 0);
	
	int i;
	for(i=0; i<4; i++)
		game_sidebar_label_build[i]=ui_widget_create_label(font_std, building[i+BUILDING_SCOUT].name);
	game_sidebar_label_build[4]=ui_widget_create_label(font_std, T("Battle support"));
	
	for(i=0; i<5; i++) {
		game_sidebar_button_build[i]=ui_widget_create_button(game_sidebar_label_build[i]);
		ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_button_build[i], 0);
		game_sidebar_button_build[i]->event_handler->add(game_sidebar_button_build[i], game_sidebar_button_build_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	}
	game_sidebar_progress_build=ui_widget_create_progressbar(font_std);
	
	game_sidebar_status.spacer=ui_widget_create_spacer();
	game_sidebar_status.label_name=ui_widget_create_label(font_std, "name");
	game_sidebar_status.progress_shield=ui_widget_create_progressbar(font_std);
	game_sidebar_status.progress_health=ui_widget_create_progressbar(font_std);
	game_sidebar_status.label_shield=ui_widget_create_label(font_std, T("Shields"));
	game_sidebar_status.label_health=ui_widget_create_label(font_std, T("Health"));
	
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.spacer, 1);
	
	if(platform.platform&DARNIT_PLATFORM_PANDORA)
		ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_minimap, 0);
	
	/*Special buildings for control points*/
	panelist_game_specialbar.pane=ui_pane_create(config.screen_w-SIDEBAR_WIDTH*2, game_sidebar_button_build[4]->y-64, SIDEBAR_WIDTH, 128, ui_widget_create_vbox());
	panelist_game_specialbar.next=&panelist_game_abilitybar;
	
	for(i=0; i<3; i++) {
		game_specialbar_label_build[i]=ui_widget_create_label(font_std, building[i+BUILDING_BUILDSITE].name);
		game_specialbar_button_build[i]=ui_widget_create_button(game_specialbar_label_build[i]);
		ui_vbox_add_child(panelist_game_specialbar.pane->root_widget, game_specialbar_button_build[i], 0);
		game_specialbar_button_build[i]->event_handler->add(game_specialbar_button_build[i], game_sidebar_button_build_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	}
	
	/*Special abilities*/
	UI_WIDGET *iconwidget;
	panelist_game_abilitybar.pane=ui_pane_create(2, 64, 52, 128+44, ui_widget_create_vbox());
	panelist_game_abilitybar.next=NULL;
	ability[0].name=T("Flare");
	ability[0].icon=d_render_tilesheet_load("res/flare.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	ability[0].action=NULL;
	iconwidget=ui_widget_create_imageview_file("res/flare.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	iconwidget->render=game_abilitybar_icon_render;
	ability[0].button=ui_widget_create_button(iconwidget);
	ability[0].button->event_handler->add(ability[0].button, game_abilitybar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ability[0].ready=100;
	ability[1].name=T("Nuke");
	ability[1].icon=d_render_tilesheet_load("res/nuke.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	ability[1].action=NULL;
	iconwidget=ui_widget_create_imageview_file("res/nuke.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	iconwidget->render=game_abilitybar_icon_render;
	ability[1].button=ui_widget_create_button(iconwidget);
	ability[1].button->enabled=0;
	ability[1].button->event_handler->add(ability[1].button, game_abilitybar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ability[1].ready=-1;
	ability[2].name=T("Radar");
	ability[2].icon=d_render_tilesheet_load("res/radar.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	ability[2].action=NULL;
	iconwidget=ui_widget_create_imageview_file("res/radar.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	iconwidget->render=game_abilitybar_icon_render;
	ability[2].button=ui_widget_create_button(iconwidget);
	ability[2].button->enabled=0;
	ability[2].button->event_handler->add(ability[2].button, game_abilitybar_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ability[2].ready=-1;
	for(i=0; i<3; i++) {
		ui_vbox_add_child(panelist_game_abilitybar.pane->root_widget, ability[i].button, 0);
		ability[i].text=d_text_surface_new(font_std, 4, 64, ability[i].button->x, ability[i].button->y+24);
	}
	
	ui_event_global_add(game_view_mouse_release, UI_EVENT_TYPE_MOUSE_RELEASE);
}

void game_abilitybar_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int i;
	for(i=0; i<3; i++)
		if(widget==ability[i].button) {
			building_place=building_place==PLACE_FLARE-i?-1:PLACE_FLARE-i;
			break;
		}
	
	ui_selected_widget=NULL;
}

void game_sidebar_minimap_mouse_down(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int x=(map_w*(e->mouse->x-widget->x)/widget->w);
	int y=(map_h*(e->mouse->y-widget->y)/widget->h);
	int map_offset=(y/map->layer->tile_h)*map->layer->tilemap->w+(x/map->layer->tile_w)%map->layer->tilemap->w;
	x-=platform.screen_w/2;
	y-=platform.screen_h/2;
	ability_place|=game_ability_place(map_offset);
	if(!ability_place) {
		d_map_camera_move(map, x, y);
		map_minimap_update_viewport();
	}
}

/* building_ready values:
 *  - negative if a building is in progress
 *  - positive if a building is ready
 *  - zero (BUILDING_NONE) if no building is ready */
void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(e&&e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_RIGHT)
		building_cancel=1;
	UI_PROPERTY_VALUE v;
	int i=0;
	// Find the building number.
	for(; i<4&&widget!=game_sidebar_button_build[i]; i++);
	if(i==4) {
		//Special buildings
		for(; i<7&&widget!=game_specialbar_button_build[i-4]; i++);
		if(i==7) {
			panelist_game_sidebar.next=panelist_game_sidebar.next==&panelist_game_abilitybar?&panelist_game_specialbar:&panelist_game_abilitybar;
			ui_selected_widget=NULL;
			return;
		}
	}
	if(!building_cancel) {
		if(building_ready==BUILDING_SCOUT+i) {
			// If the building is ready, place it.
			building_place=building_ready;
		} else if(building_ready==BUILDING_NONE) { 
			//Build if no there are no buildings in progress nor other buildings ready.
			client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+i, MSG_BUILDING_START, NULL);
			v.p=game_sidebar_progress_build;
			widget->set_prop(widget, UI_BUTTON_PROP_CHILD, v);
			game_set_building_progress(0, 0);
			game_set_building_ready(-(BUILDING_SCOUT+i));
		}
	} else if(ABS(building_ready)==BUILDING_SCOUT+i) {
		//Cancel if the building is is in progress or ready and shift is pressed
		client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+i, MSG_BUILDING_STOP, NULL);
		if(i<4)
			v.p=game_sidebar_label_build[i];
		else {
			v.p=game_specialbar_label_build[i-4];
			panelist_game_sidebar.next=&panelist_game_abilitybar;
		}
		widget->set_prop(widget, UI_BUTTON_PROP_CHILD, v);
		game_set_building_progress(0, 0);
		game_set_building_ready(BUILDING_NONE); // No buildings in progress
	}
	ui_selected_widget=NULL;
}

void game_view_key_press(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	//TODO: keysyms of number keys might not be sequential on all platforms
	if(e->keyboard->keysym>=KEY(1)&&e->keyboard->keysym<=KEY(5)) {
		game_sidebar_button_build_click(game_sidebar_button_build[e->keyboard->keysym-KEY(1)], UI_EVENT_TYPE_UI_WIDGET_ACTIVATE, NULL);
	}
}

void game_view_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	int scroll_x=0, scroll_y=0;
	int screen_w=platform.screen_w, screen_h=platform.screen_h;
	
	//darnit buttons have no press or release events, we have to handle this ourselves
	
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
			
		if(e->buttons->l) {
			scroll_x*=2;
			scroll_y*=2;
		}
		if(scroll_x||scroll_y) {
			d_map_camera_move(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
			map_minimap_update_viewport();
		}
	}
	if(!(platform.platform&DARNIT_PLATFORM_MAEMO)||e->buttons->select) {
		//Interaction keys
		if(e->buttons->start&&!prevbuttons.start)
			game_state(GAME_STATE_GAME_MENU);
		if(e->buttons->a&&!prevbuttons.a)
			chat_toggle(&panelist_game_sidebar);
		if(e->buttons->b&&!prevbuttons.b) {
			if(building_place>-1) {
				building_place=-1;
			} else if(map_selected_building()) {
				client_message_send(player_id, MSG_SEND_PLACE_BUILDING, BUILDING_NONE, map_selected_index(), NULL);
				map_select_nothing();
			}
		}
		if(e->buttons->x&&!prevbuttons.x) {
			DARNIT_MOUSE m=d_mouse_get();
			UI_EVENT_MOUSE e_m={
				0,
				0,
				UI_EVENT_MOUSE_BUTTON_LEFT,
				0,
			};
			e_m.x=m.x; e_m.y=m.y;
			UI_EVENT e={.mouse=&e_m};
			ui_event_global_send(UI_EVENT_TYPE_MOUSE_PRESS, &e);
		}
	}
	
	// If shift is pressed (button Y) set building_cancel
	building_cancel = e->buttons->l;
	
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
	
	d_map_camera_move(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
	map_minimap_update_viewport();
}

void game_view_mouse_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	//Make sure there is no pane in the way
	if(e->mouse->x>platform.screen_w-SIDEBAR_WIDTH)
		return;
	if(panelist_game_sidebar.next==&panelist_game_specialbar&&PINR(e->mouse->x, e->mouse->y, panelist_game_specialbar.pane->x, panelist_game_specialbar.pane->y, panelist_game_specialbar.pane->w, panelist_game_specialbar.pane->h))
		return;
	if(chat_is_visible(&panelist_game_sidebar)&&PINR(e->mouse->x, e->mouse->y, panelist_chat.pane->x, panelist_chat.pane->y, panelist_chat.pane->w, panelist_chat.pane->h))
		return;
	if(PINR(e->mouse->x, e->mouse->y, panelist_game_abilitybar.pane->x, panelist_game_abilitybar.pane->y, panelist_game_abilitybar.pane->w, panelist_game_abilitybar.pane->h))
		return ;
	
	if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_RIGHT)
		building_place=-1;
	else if(e->mouse->buttons&UI_EVENT_MOUSE_BUTTON_LEFT) {
		int map_offset=((e->mouse->y+map->cam_y)/map->layer->tile_h)*map->layer->tilemap->w+((e->mouse->x+map->cam_x)/map->layer->tile_w)%map->layer->tilemap->w;
		if(map_offset<0||map_offset>map->layer->tilemap->w*map->layer->tilemap->h)
			return;
		if(building_place>-1) {
			client_message_send(player_id, MSG_SEND_PLACE_BUILDING, building_place, map_offset, NULL);
			building_place=-1;
		} else if(attacker_target) {
			client_message_send(player_id, MSG_SEND_SET_ATTACK, map_selected_index(), map_offset, NULL);
			attacker_target=0;
		} else if(!game_ability_place(map_offset)) {
			//status selected clicked building, etc
			int selected_building=map_selected_building();
			if(map_offset==map_selected_index()&&map_selected_owner()==player_id&&(selected_building==BUILDING_ATTACKER||selected_building==BUILDING_SCOUT))
				attacker_target=1;
			else
				map_select_building(map_offset);
		}
	}
}

void game_view_mouse_release(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	ability_place=0;
}

void game_update_building_status() {
	ui_vbox_remove_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.progress_health);
	ui_vbox_remove_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.label_health);
	ui_vbox_remove_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.progress_shield);
	ui_vbox_remove_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.label_shield);
	ui_vbox_remove_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.label_name);
	//ui_vbox_remove_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.spacer);
	if(map_selected_index()<0)
		return;
	
	if(platform.platform&DARNIT_PLATFORM_PANDORA)
		ui_vbox_remove_child(panelist_game_sidebar.pane->root_widget, game_sidebar_minimap);
	
	//ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.spacer, 1);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.label_name, 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.label_shield, 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.progress_shield, 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.label_health, 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_status.progress_health, 0);
	if(platform.platform&DARNIT_PLATFORM_PANDORA)
		ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_minimap, 0);
	UI_PROPERTY_VALUE v;
	v.p=(void *)building[map_selected_building()].name;
	
	game_sidebar_status.label_name->set_prop(game_sidebar_status.label_name, UI_LABEL_PROP_TEXT, v);
	v.i=map_get_building_shield(map_selected_index());
	game_sidebar_status.progress_shield->set_prop(game_sidebar_status.progress_shield, UI_PROGRESSBAR_PROP_PROGRESS, v);
	v.i=map_get_building_health(map_selected_index());
	game_sidebar_status.progress_health->set_prop(game_sidebar_status.progress_health, UI_PROGRESSBAR_PROP_PROGRESS, v);
}

void game_view_scroll_to(int x, int y) {
	d_map_camera_move(map, x*map->layer[map->layers-2].tile_w-(platform.screen_w-SIDEBAR_WIDTH)/2, y*map->layer[map->layers-2].tile_h-platform.screen_h/2);
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
	for(i=0; i<3; i++) {
		UI_PROPERTY_VALUE v={.p=game_specialbar_label_build[i]};
		game_specialbar_button_build[i]->set_prop(game_specialbar_button_build[i], UI_BUTTON_PROP_CHILD, v);
	}
	if(building_place>-1)
		building_place=-1;
}

void game_set_building_ready(int building) {
	building_ready=building;
}

int game_ability_place(int index) {
	switch(building_place) {
		case PLACE_FLARE:
			client_message_send(player_id, MSG_SEND_SET_FLARE, 0, index, NULL);
			break;
		case PLACE_NUKE:
			client_message_send(player_id, MSG_SEND_CP_DEPLOY, BUILDING_CLUSTERBOMB, index, NULL);
			break;
		case PLACE_RADAR:
			client_message_send(player_id, MSG_SEND_CP_DEPLOY, BUILDING_RADAR, index, NULL);
			break;
		default:
			return 0;
	}
	building_place=-1;
	return 1;
}

void game_attacklist_lines_recalculate() {
	struct GAME_ATTACKLIST *l;
	int i, x1, x2, y1, y2;
	int map_wt=map->layer[map->layers-2].tilemap->w;
	//int map_ht=map->layer[map->layers-2].tilemap->h;
	int tw=map->layer[map->layers-2].tile_w;
	int th=map->layer[map->layers-2].tile_h;
	for(i=0; i<MAX_PLAYERS; i++) {
		game_attacklist_render[i].lines=d_render_line_free(game_attacklist_render[i].lines);
		if(i<players)
			game_attacklist_render[i].lines=d_render_line_new(game_attacklist_render[i].length, 1);
	}
	
	int *j=calloc(players, sizeof(int));
	
	for(l=game_attacklist; l; l=l->next) {
		x1=l->index%map_wt*tw+tw/2;
		y1=l->index/map_wt*th+th/2;
		x2=l->target%map_wt*tw+tw/2;
		y2=l->target/map_wt*th+th/2;
		d_render_line_move(game_attacklist_render[l->owner].lines, j[l->owner], x1, y1, x2, y2);
		j[l->owner]++;
	}
	free(j);
}

void game_attacklist_add(int index) {
	struct GAME_ATTACKLIST **l, *ll;
	for(l=&(game_attacklist); *l; l=&((*l)->next));
	if((*l=malloc(sizeof(struct GAME_ATTACKLIST)))==NULL)
		return;
	ll=*l;
	ll->index=index;
	ll->target=index;
	ll->owner=(map->layer[map->layers-2].tilemap->data[index]&0xFFFF)/tilesx-1;
	ll->next=NULL;
	game_attacklist_render[ll->owner].length++;
	game_attacklist_lines_recalculate();
}

void game_attacklist_remove(int index) {
	struct GAME_ATTACKLIST **l, *l_next;
	for(l=&(game_attacklist); *l; l=&((*l)->next)) {
		if((*l)->index==index) {
			l_next=(*l)->next;
			int owner=(*l)->owner;
			free(*l);
			*l=l_next;
			game_attacklist_render[owner].length--;
			game_attacklist_lines_recalculate();
			break;
		}
	}
}

void game_attacklist_clear() {
	struct GAME_ATTACKLIST *l, *next;
	int i;
	for(l=game_attacklist; l; l=next) {
		next=l->next;
		free(l);
	}
	game_attacklist=NULL;
	for(i=0; i<MAX_PLAYERS; i++)  {
		game_attacklist_render[i].length=0;
		game_attacklist_render[i].lines=d_render_line_free(game_attacklist_render[i].lines);
	}
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
			l->target=target;
			game_attacklist_lines_recalculate();
			break;
		}
	}
}

void game_abilitybar_icon_render(UI_WIDGET *widget) {
	//Override for imageview render. tinting the icon if ability is inactive
	struct UI_IMAGEVIEW_PROPERTIES *p=widget->properties;
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	int i=-1;
	UI_PROPERTY_VALUE v={.p=NULL};
	while(v.p!=widget) {
		i++;
		v=ability[i].button->get_prop(ability[i].button, UI_BUTTON_PROP_CHILD);
	}
		
	if(ability[i].ready==100)
		d_render_tint(255, 255, 255, 255);
	else
		d_render_tint(95, 95, 95, 255);
	
	d_render_tile_draw(p->tile, 1);
	d_render_tint(255, 255, 255, 255);
	if(ability[i].ready>=0&&ability[i].ready<100) {
		d_render_blend_enable();
		d_text_surface_draw(ability[i].text);
		d_render_blend_disable();
	}
	d_render_tint(r, g, b, a);
	d_render_line_draw(p->border, 4);
}

void game_view_draw() {
	view_background_draw();
	int selected=map_selected_building();
	if((building_place>-1||((selected==BUILDING_PYLON||selected==BUILDING_GENERATOR)&&map_selected_owner()==player_id)))
		map_draw(1);
	else
		map_draw(0);
	
	int i=game_attacklist_blink_semaphore/(4*players);
	d_render_offset(map->cam_x, map->cam_y);
	d_render_tint(player_color[i].r, player_color[i].g, player_color[i].b, 255);
	if(game_attacklist_render[i].lines)
		d_render_line_draw(game_attacklist_render[i].lines, game_attacklist_render[i].length);
	d_render_offset(0, 0);
	d_render_tint(255, 255, 255, 255);
	
	game_attacklist_blink_semaphore++;
	if(game_attacklist_blink_semaphore>=(16*players))
		game_attacklist_blink_semaphore=0;
}

void game_mouse_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(255, 255, 255, 255);
	if(building_place!=-1&&e->mouse->x<platform.screen_w-SIDEBAR_WIDTH) {
		DARNIT_MAP_LAYER *l=&map->layer[map->layers-1];
		int x=(e->mouse->x+map->cam_x)/l->tile_w*l->tile_w;
		int y=(e->mouse->y+map->cam_y)/l->tile_h*l->tile_h;
		d_render_offset(map->cam_x, map->cam_y);
		d_render_blend_enable();
		if(building_place<=-1)
			d_render_tile_blit(ability[-building_place-2].icon, 0, x, y);
		else if(building_place>-1)
			d_render_tile_blit(l->ts, ((player_id+1)*tilesx)+building_place-1, x, y);
		d_render_blend_disable();
		d_render_offset(0, 0);
	}
	d_render_tint(r, g, b, a);
	if(attacker_target)
		game_mouse_target_draw(widget, type, e);
	/*else
		view_mouse_draw(widget, type, e);*/
}

void game_mouse_target_draw(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	unsigned char r, g, b, a;
	d_render_tint_get(&r, &g, &b, &a);
	d_render_tint(255, 255, 255, 255);
	d_render_blend_enable();
	d_render_tile_blit(mouse_target_tilesheet, 0, e->mouse->x-16, e->mouse->y-16);
	d_render_blend_disable();
	d_render_tint(r, g, b, a);
}
