#include "muon.h"
#include "client.h"
#include "map.h"
#include "game.h"

void game_view_init() {
	building_place=-1;
	
	panelist_game_sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, platform.screen_h, NULL);
	ui_pane_set_root_widget(panelist_game_sidebar.pane, ui_widget_create_vbox());
	panelist_game_sidebar.next=NULL;
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, "Muon\n===="), 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, "Buildings:"), 0);
	game_sidebar_label_build[0]=ui_widget_create_label(font_std, "Scout");
	game_sidebar_label_build[1]=ui_widget_create_label(font_std, "Attacker");
	game_sidebar_label_build[2]=ui_widget_create_label(font_std, "Pylon");
	game_sidebar_label_build[3]=ui_widget_create_label(font_std, "Wall");
	int i;
	for(i=0; i<4; i++) {
		game_sidebar_button_build[i]=ui_widget_create_button(game_sidebar_label_build[i]);
		ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_button_build[i], 0);
		game_sidebar_button_build[i]->event_handler->add(game_sidebar_button_build[i], game_sidebar_button_build_click, UI_EVENT_TYPE_UI);
	}
	game_sidebar_progress_build=ui_widget_create_progressbar(font_std);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_spacer(), 1);
	game_sidebar_progress_shield=ui_widget_create_progressbar(font_std);
	game_sidebar_progress_health=ui_widget_create_progressbar(font_std);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, "Shield"), 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_progress_shield, 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, "Health"), 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_progress_health, 0);
}

void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;
	int i, do_build=-1;
	UI_PROPERTY_VALUE wv=widget->get_prop(widget, UI_BUTTON_PROP_CHILD);
	UI_PROPERTY_VALUE pv=game_sidebar_progress_build->get_prop(game_sidebar_progress_build, UI_PROGRESSBAR_PROP_PROGRESS);
	for(i=0; i<4; i++) {
		if(widget==game_sidebar_button_build[i]&&wv.p!=game_sidebar_progress_build) {
			do_build=i; //just make sure to cancel all other buildings first
		} else if(widget==game_sidebar_button_build[i]&&pv.i==100) {
			printf("Place building\n");
			building_place=i;
		} else {
			UI_PROPERTY_VALUE v={.p=game_sidebar_label_build[i]};
			game_sidebar_button_build[i]->set_prop(game_sidebar_button_build[i], UI_BUTTON_PROP_CHILD, v);
			client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+i, MSG_BUILDING_STOP, NULL);
		}
	}
	if(do_build!=-1) {
		client_message_send(player_id, MSG_SEND_START_BUILD, BUILDING_SCOUT+do_build, MSG_BUILDING_START, NULL);
		UI_PROPERTY_VALUE v={.p=game_sidebar_progress_build};
		widget->set_prop(widget, UI_BUTTON_PROP_CHILD, v);
		v.i=0;
		game_sidebar_progress_build->set_prop(game_sidebar_progress_build, UI_PROGRESSBAR_PROP_PROGRESS, v);
	}
}

void game_view_mouse_move(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	//Map movement should be broken out to separate function, keyboard movement should be broken out to separate handler
	DARNIT_KEYS keys=darnitButtonGet();
	DARNIT_KEYS *buttons=&keys;
	int scroll_x=0, scroll_y=0;
	int screen_w=platform.screen_w, screen_h=platform.screen_h;
	
	if((e->mouse->x<SCROLL_OFFSET||buttons->left)&&map->cam_x>-((screen_w-SIDEBAR_WIDTH)/2))
		scroll_x=-SCROLL_SPEED;
	else if((e->mouse->x>platform.screen_w-SCROLL_OFFSET||buttons->right)&&map->cam_x<map_w-(screen_w-SIDEBAR_WIDTH)/2)
		scroll_x=SCROLL_SPEED;
	if((e->mouse->y<SCROLL_OFFSET||buttons->up)&&map->cam_y>-(screen_h)/2)
		scroll_y=-SCROLL_SPEED;
	else if((e->mouse->y>platform.screen_h-SCROLL_OFFSET||buttons->down)&&map->cam_y<map_h-screen_h/2)
		scroll_y=SCROLL_SPEED;
		
	if(buttons->y) {
		scroll_x*=2;
		scroll_y*=2;
	}
	darnitMapCameraMove(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
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
			client_message_send(player_id, MSG_SEND_PLACE_BUILDING, BUILDING_SCOUT+building_place, map_offset, NULL);
			building_place=-1;
		} else {
			//status about clicked building, etc
			UI_PROPERTY_VALUE v;
			v.i=map_get_building_shield(map_offset);
			game_sidebar_progress_shield->set_prop(game_sidebar_progress_shield, UI_PROGRESSBAR_PROP_PROGRESS, v);
			v.i=map_get_building_health(map_offset);
			game_sidebar_progress_health->set_prop(game_sidebar_progress_health, UI_PROGRESSBAR_PROP_PROGRESS, v);
			
			map_select_building(map_offset);
		}
	}
}

void game_view_scroll_to(int x, int y) {
	darnitMapCameraMove(map, x*map->layer[map->layers-2].tile_w-(platform.screen_w-SIDEBAR_WIDTH)/2, x*map->layer[map->layers-2].tile_h-platform.screen_h/2);
}

void game_set_building_progress(int building, int progress) {
	UI_PROPERTY_VALUE v;
	v.i=progress;
	game_sidebar_progress_build->set_prop(game_sidebar_progress_build, UI_PROGRESSBAR_PROP_PROGRESS, v);
}

void game_reset_building_progress() {
	int i;
	for(i=0; i<4; i++) {
		UI_PROPERTY_VALUE v={.p=game_sidebar_label_build[i]};
		game_sidebar_button_build[i]->set_prop(game_sidebar_button_build[i], UI_BUTTON_PROP_CHILD, v);
	}
}

void game_view_draw() {
	int selected=map_selected_building();	
	if((building_place!=-1||selected==BUILDING_PYLON||selected==BUILDING_GENERATOR))
		map_draw(1);
	else
		map_draw(0);
}

void game_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	DARNIT_MAP_LAYER *l=&map->layer[map->layers-1];
	darnitRenderOffset(map->cam_x, map->cam_y);
	darnitRenderBlendingEnable();
	if(building_place!=-1) {
		darnitRenderTileBlit(l->ts, player_id*8+building_place+BUILDING_SCOUT+7, (e->mouse->x+map->cam_x)/l->tile_w*l->tile_w, (e->mouse->y+map->cam_y)/l->tile_h*l->tile_h);
	}
	darnitRenderBlendingDisable();
	darnitRenderOffset(0, 0);
	view_draw_mouse(widget, type, e);
}
