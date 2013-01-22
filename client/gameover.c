#include "muon.h"
#include "view.h"
#include "game.h"
#include "menu/menu.h"
#include "gameover.h"

void gameover_init() {
	panelist_gameover_sidebar.pane=ui_pane_create(platform.screen_w-SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, platform.screen_h, ui_widget_create_vbox());
	panelist_gameover_sidebar.next=&panelist_gameover_stats;
	panelist_gameover_stats.pane=ui_pane_create((platform.screen_w-SIDEBAR_WIDTH)/2-320/2, platform.screen_h/2-240/2, 320, 240, ui_widget_create_vbox());
	panelist_gameover_stats.next=NULL;
	
	gameover_sidebar_button_menu=ui_widget_create_button_text(font_std, T("Menu"));
	gameover_sidebar_button_menu->event_handler->add(gameover_sidebar_button_menu, gameover_sidebar_button_menu_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	ui_vbox_add_child(panelist_gameover_sidebar.pane->root_widget, ui_widget_create_label(font_std, T("Game over")), 1);
	ui_vbox_add_child(panelist_gameover_sidebar.pane->root_widget, ui_widget_create_spacer(), 1);
	ui_vbox_add_child(panelist_gameover_sidebar.pane->root_widget, gameover_sidebar_button_menu, 0);
}

void gameover_sidebar_button_menu_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	game_state(GAME_STATE_MENU);
	menu_state(MENU_STATE_MAIN);
}
