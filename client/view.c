#include <string.h>

#include "muon.h"
#include "view.h"

void view_init() {
	font_std=darnitFontLoad("../res/FreeMonoBold.ttf", 12, 512, 512);
	
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
	
	//Countdown
	panelist_countdown.pane=ui_pane_create(platform.screen_w/2-64, platform.screen_h/2-32, 128, 64, NULL);
	strcpy(countdown_text, "Downloading map");
	ui_pane_set_root_widget(panelist_countdown.pane, ui_widget_create_label(font_std, countdown_text));
	panelist_game_sidebar.next=NULL;
	
	//Game
	panelist_game_sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, platform.screen_h, NULL);
	ui_pane_set_root_widget(panelist_game_sidebar.pane, ui_widget_create_vbox());
	panelist_game_sidebar.next=NULL;
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, "Muon\n===="), 0);
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, ui_widget_create_label(font_std, "Buildings:"), 0);
	game_sidebar_button_build_1=ui_widget_create_button(ui_widget_create_label(font_std, "Repeater"));
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_button_build_1, 0);
	game_sidebar_button_build_1->event_handler->add(game_sidebar_button_build_1, game_sidebar_button_build_click, UI_EVENT_TYPE_UI);
	game_sidebar_button_build_2=ui_widget_create_button(ui_widget_create_label(font_std, "Scout"));
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_button_build_2, 0);
	game_sidebar_button_build_2->event_handler->add(game_sidebar_button_build_2, game_sidebar_button_build_click, UI_EVENT_TYPE_UI);
	game_sidebar_button_build_3=ui_widget_create_button(ui_widget_create_label(font_std, "Pizza hut"));
	ui_vbox_add_child(panelist_game_sidebar.pane->root_widget, game_sidebar_button_build_3, 0);
	game_sidebar_button_build_3->event_handler->add(game_sidebar_button_build_3, game_sidebar_button_build_click, UI_EVENT_TYPE_UI);
}

void view_scroll(DARNIT_MOUSE mouse) {
		register int scroll_x=0, scroll_y=0;
		
		if(mouse.x<SCROLL_OFFSET&&map->cam_x>0)
			scroll_x=-SCROLL_SPEED;
		else if(mouse.x>platform.screen_w-SIDEBAR_WIDTH-SCROLL_OFFSET&&mouse.x<platform.screen_w-SIDEBAR_WIDTH&&map->cam_x<map_w-platform.screen_w+SIDEBAR_WIDTH)
			scroll_x=SCROLL_SPEED;
		if(mouse.y<SCROLL_OFFSET&&map->cam_y>0)
			scroll_y=-SCROLL_SPEED;
		else if(mouse.y>platform.screen_h-SCROLL_OFFSET&&map->cam_y<map_h-platform.screen_h)
			scroll_y=SCROLL_SPEED;
		darnitMapCameraMove(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
}

void view_draw() {
	darnitRenderTilemap(map->layer->tilemap);
}
