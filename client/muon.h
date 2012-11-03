#ifndef MUON_H
#define MUON_H

#include <stdio.h>
#include <stdlib.h>

#include <darnit/darnit.h>

#include "platform.h"
#include "ui/ui.h"
#include "view.h"

#define SCROLL_OFFSET 8
#define SCROLL_SPEED 4

DARNIT_MAP *map;
int map_w, map_h;
char player_name[32];
int building_place;
int home_x, home_y;

typedef enum {
	GAME_STATE_INPUT_NAME,
	GAME_STATE_CONNECT_SERVER,
	GAME_STATE_CONNECTING,
	GAME_STATE_LOBBY,
	GAME_STATE_GAME,
	GAME_STATE_GAME_MENU,
	GAME_STATE_QUIT,
} GAME_STATE;

GAME_STATE gamestate;
static struct UI_PANE_LIST *gamestate_pane[7]={
	&panelist_input_name,
	&panelist_connect_server,
	&panelist_connecting,
	&panelist_lobby,
	&panelist_game_sidebar,
	&panelist_game_menu,
	NULL,
};

void game_state(GAME_STATE state);

#endif
