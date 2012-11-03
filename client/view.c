#include <string.h>

#include "muon.h"
#include "view.h"
#include "game.h"
//#include "client.h"
//#include "engine.h"
#include "chat.h"
#include "menu.h"
#include "lobby.h"

void view_init() {
	//TODO: lots of breaking out to separate functions, game menu and lobby for example
	font_std=darnitFontLoad("../res/FreeMonoBold.ttf", 12, 512, 512);
	mouse_tilesheet=darnitRenderTilesheetLoad("../res/mouse.png", 16, 16, DARNIT_PFORMAT_RGB5A1);
	
	menu_init();
	lobby_init();
	game_view_init();
	
	gamestate_pane[0]=&panelist_input_name;
	gamestate_pane[1]=&panelist_connect_server;
	gamestate_pane[2]=&panelist_connecting;
	gamestate_pane[3]=&panelist_lobby;
	gamestate_pane[4]=&panelist_game_sidebar;
	gamestate_pane[5]=&panelist_game_menu;
	gamestate_pane[6]=NULL;
}

void view_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	float r, g, b, a;
	darnitRenderTintGet(&r, &g, &b, &a);
	darnitRenderTint(1, 1, 1, 1);
	darnitRenderBlendingEnable();
	darnitRenderTileBlit(mouse_tilesheet, 0, e->mouse->x, e->mouse->y);
	darnitRenderBlendingDisable();
	darnitRenderTint(r, g, b, a);
}
