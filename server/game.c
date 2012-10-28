#include "server.h"


int gameInit() {
	server->game.started = 0;

	return 0;
}


int gameKillPlayerUnits(unsigned int player) {
	return 0;
}


int gameAttemptStart() {
	int i;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_LOBBY)
			continue;
		if (server->player[i].map_progress < 100)
			return -1;
		if (server->player[i].status != PLAYER_READY_TO_START)
			return -1;
	}

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_READY_TO_START) {
			playerDisconnect(i);
			continue;
		}

		server->player[i].status = PLAYER_IN_GAME;
	}

	playerMessageBroadcast(0, MSG_SEND_GAME_START, 0, 0, NULL);
	server->game.started = 1;

	return 0;
}
