#ifndef MUON_H
#define MUON_H

#include <stdio.h>
#include <stdlib.h>

#include <darnit/darnit.h>

#include "ui/ui.h"

#define SCROLL_OFFSET 8
#define SCROLL_SPEED 4

DARNIT_PLATFORM platform;

DARNIT_MAP *map;
int map_w, map_h;
char player_name[32];
int building_place;
int home_x, home_y;

typedef enum {
	GAME_STATE_INPUT_NAME,
	GAME_STATE_CONNECT_SERVER,
	GAME_STATE_CONNECTING,
	GAME_STATE_COUNTDOWN,
	GAME_STATE_GAME,
	GAME_STATE_GAME_MENU,
	GAME_STATE_QUIT,
} GAME_STATE;

GAME_STATE state;

#endif
