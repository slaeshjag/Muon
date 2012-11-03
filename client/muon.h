#ifndef MUON_H
#define MUON_H

#include <stdio.h>
#include <stdlib.h>

#include <darnit/darnit.h>
#include "../server/server_api/server.h"

#include "platform.h"
#include "ui/ui.h"
#include "view.h"

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
struct UI_PANE_LIST *gamestate_pane[7];

void game_state(GAME_STATE state);

#endif
