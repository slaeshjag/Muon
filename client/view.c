#include <string.h>

#include "muon.h"
#include "view.h"
#include "client.h"
#include "engine.h"

void view_init() {
	font_std=darnitFontLoad("../res/FreeMonoBold.ttf", 12, 512, 512);
	mouse_tilesheet=darnitRenderTilesheetLoad("../res/mouse.png", 16, 16, DARNIT_PFORMAT_RGBA8);
	building_place=-1;
	powergrid=NULL;
	powergrid_lines=0;
	
	//Input player name
	panelist_input_name.pane=ui_pane_create(16, 16, 256, 96, NULL);
	ui_pane_set_root_widget(panelist_input_name.pane, ui_widget_create_vbox());
	panelist_input_name.next=NULL;
	ui_vbox_add_child(panelist_input_name.pane->root_widget, ui_widget_create_label(font_std, "Enter your player name:"), 1);
	input_name_entry=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_input_name.pane->root_widget, input_name_entry, 0);
	input_name_button=ui_widget_create_button(ui_widget_create_label(font_std, "OK"));
	ui_vbox_add_child(panelist_input_name.pane->root_widget, input_name_button, 0);
	input_name_button->event_handler->add(input_name_button, input_name_button_click, UI_EVENT_TYPE_UI);
	
	//Connect to server
	UI_PROPERTY_VALUE v={.p=NULL};
	panelist_connect_server.pane=ui_pane_create(16, 16, 256, 128, NULL);
	ui_pane_set_root_widget(panelist_connect_server.pane, ui_widget_create_vbox());
	panelist_connect_server.next=NULL;
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, ui_widget_create_label(font_std, "Connect to a server"), 1);
	connect_server_entry_host=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_entry_host, 0);
	connect_server_entry_port=ui_widget_create_entry(font_std);
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_entry_port, 0);
	connect_server_button=ui_widget_create_button(ui_widget_create_label(font_std, "Connect"));
	ui_vbox_add_child(panelist_connect_server.pane->root_widget, connect_server_button, 0);
	connect_server_button->event_handler->add(connect_server_button, connect_server_button_click, UI_EVENT_TYPE_UI);
	v.p="localhost";
	connect_server_entry_host->set_prop(connect_server_entry_host, UI_ENTRY_PROP_TEXT, v);
	v.p="1337";
	connect_server_entry_port->set_prop(connect_server_entry_port, UI_ENTRY_PROP_TEXT, v);
	
	//Connecting
	panelist_connecting.pane=ui_pane_create(platform.screen_w/2-90, platform.screen_h/2-32, 180, 64, NULL);
	panelist_connecting.next=NULL;
	ui_pane_set_root_widget(panelist_connecting.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_connecting.pane->root_widget, ui_widget_create_label(font_std, "Connecting to server"), 1);
	connecting_button_cancel=ui_widget_create_button_text("Cancel");
	ui_vbox_add_child(panelist_connecting.pane->root_widget, connecting_button_cancel, 0);
	connecting_button_cancel->event_handler->add(connecting_button_cancel, connecting_button_cancel_click, UI_EVENT_TYPE_UI);
	
	//Countdown
	panelist_countdown.pane=ui_pane_create(platform.screen_w/2-64, platform.screen_h/2-32, 128, 64, NULL);
	strcpy(countdown_text, "Downloading map");
	ui_pane_set_root_widget(panelist_countdown.pane, ui_widget_create_vbox());
	countdown_label=ui_widget_create_label(font_std, countdown_text);
	pbar=ui_widget_create_progressbar(font_std);
	ui_vbox_add_child(panelist_countdown.pane->root_widget, countdown_label, 1);
	ui_vbox_add_child(panelist_countdown.pane->root_widget, pbar, 0);
	v.i=0;
	pbar->set_prop(pbar, UI_PROGRESSBAR_PROP_PROGRESS, v);	
	panelist_countdown.next=&panelist_countdown_ready;
	panelist_countdown_ready.pane=ui_pane_create(16, 16, 64, 32, NULL);
	ui_pane_set_root_widget(panelist_countdown_ready.pane, ui_widget_create_hbox());
	countdown_ready=ui_widget_create_checkbox();
	ui_hbox_add_child(panelist_countdown_ready.pane->root_widget, countdown_ready, 0);
	ui_hbox_add_child(panelist_countdown_ready.pane->root_widget, ui_widget_create_label(font_std, "Ready"), 1);
	panelist_countdown_ready.next=NULL;
	
	//Game
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
	
	//Game menu
	panelist_game_menu.pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-128, 256, 256, NULL);
	ui_pane_set_root_widget(panelist_game_menu.pane, ui_widget_create_vbox());
	panelist_game_menu.next=NULL;
	ui_vbox_add_child(panelist_game_menu.pane->root_widget, ui_widget_create_label(font_std, "Muon\n===="), 0);
	game_menu_button[0]=ui_widget_create_button(ui_widget_create_label(font_std, "Quit game"));
	game_menu_button[1]=ui_widget_create_button(ui_widget_create_label(font_std, "Return to game"));
	for(i=0; i<2; i++) {
		ui_vbox_add_child(panelist_game_menu.pane->root_widget, game_menu_button[i], 0);
		game_menu_button[i]->event_handler->add(game_menu_button[i], game_menu_button_click, UI_EVENT_TYPE_UI);
	}
}

void view_scroll(DARNIT_MOUSE *mouse) {
		register int scroll_x=0, scroll_y=0;
		
		if(mouse->x<SCROLL_OFFSET&&map->cam_x>0)
			scroll_x=-SCROLL_SPEED;
		else if(mouse->x>platform.screen_w-SCROLL_OFFSET&&map->cam_x<map_w-platform.screen_w+SIDEBAR_WIDTH)
			scroll_x=SCROLL_SPEED;
		if(mouse->y<SCROLL_OFFSET&&map->cam_y>0)
			scroll_y=-SCROLL_SPEED;
		else if(mouse->y>platform.screen_h-SCROLL_OFFSET&&map->cam_y<map_h-platform.screen_h)
			scroll_y=SCROLL_SPEED;
		darnitMapCameraMove(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
		
		if(mouse->x>platform.screen_w-SIDEBAR_WIDTH)
			return;
		if(mouse->rmb)
			building_place=-1;
		else if(mouse->lmb&&building_place>-1) {
			int map_offset=((mouse->y+map->cam_y)/map->layer->tile_h)*map->layer->tilemap->w+((mouse->x+map->cam_x)/map->layer->tile_w)%map->layer->tilemap->w;
			printf("Building placed at %i\n", map_offset);
			//map->layer[map->layers-1].tilemap->data[map_offset]=2;
			//darnitRenderTilemapRecalculate(map->layer[map->layers-1].tilemap);
			client_message_send(player_id, MSG_SEND_PLACE_BUILDING, BUILDING_SCOUT+building_place, map_offset, NULL);
			building_place=-1;
		}else if(mouse->lmb) {
			//status about clicked building, etc
		}
		
}

void view_draw(DARNIT_MOUSE *mouse) {
	int i;
	for(i=0; i<map->layers; i++)
		darnitRenderTilemap(map->layer[i].tilemap);
	if(powergrid&&building_place!=-1) {
		darnitRenderOffset(map->cam_x, map->cam_y);
		DARNIT_MAP_LAYER *l=&map->layer[map->layers-1];
		darnitRenderBlendingEnable();
		darnitRenderTileBlit(l->ts, player_id*8+building_place+BUILDING_SCOUT+7, (mouse->x+map->cam_x)/l->tile_w*l->tile_w, (mouse->y+map->cam_y)/l->tile_h*l->tile_h);
		darnitRenderBlendingDisable();
		darnitRenderLineDraw(powergrid, powergrid_lines);
		darnitRenderOffset(0, 0);
	}
}
