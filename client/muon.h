#ifndef MUON_H
#define MUON_H

#include <stdio.h>
#include <stdlib.h>

#include <darnit/darnit.h>

#include "ui/ui.h"

#define SCROLL_OFFSET 32
#define SCROLL_SPEED 4

DARNIT_PLATFORM platform;

DARNIT_MAP *map;
int map_w, map_h;
char player_name[32];

typedef enum {
	GAME_STATE_INPUT_NAME,
	GAME_STATE_CONNECT_SERVER,
	GAME_STATE_GAME,
} GAME_STATE;

GAME_STATE state;

#endif
