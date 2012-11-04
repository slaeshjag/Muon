#include "server.h"


int gameInit(int gamespeed) {
	server->game.started = 0;
	server->game.time_elapsed = 0;
	server->game.countdown = 0;

	/* FIXME: Make this variable */
	server->game.gamespeed = gamespeed;

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
		if (server->player[i].status == PLAYER_WAITING_FOR_IDENTIFY) {
			playerDisconnect(i);
			continue;
		}

		if (server->player[i].status == PLAYER_UNUSED)
			continue;

		server->player[i].status = PLAYER_IN_GAME;
	}

	server->game.started = 1;

	return 0;
}


void gameSpawn() {
	int n, i, t, building, owner;

	n = server->w * server->h;
	for (i = 0; i < n; i++)
		if (server->map_c.tile_data[i]) {
			t = server->map_c.tile_data[i] & 0xFFF;
			if (!t)
				continue;
			building = (t % 8) + 1;
			owner = (t / 8) - 1;
			if (owner < 0 || owner >= server->players) {
				fprintf(stderr, "Bad owner\n");
				continue;
			}
			if (server->player[owner].status != PLAYER_IN_GAME) {
				fprintf(stderr, "player not in game\n");
				continue;
			}
			if (building >= UNITS_DEFINED) {
				fprintf(stderr, "Unit undefined\n");
				continue;
			}
			fprintf(stderr, "Spawning building %i for player %i at %i\n", building, owner, i);
			unitSpawn(owner, building, i % server->w, i / server->w);
		}
	fprintf(stderr, "Pulsing pylons\n");
	unitPylonPulse();
	
	return;
}


void gameStart() {
	int i;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_GAME)
			continue;
	}

	server->accept = networkSocketDisconnect(server->accept);
	gameSpawn();

	return;
}


void gameLoop(int msec) {
	server->game.time_elapsed += msec;

	if (server->game.time_elapsed < SERVER_GAME_START_DELAY) {
		if (server->game.countdown != server->game.time_elapsed / 1000) {
			server->game.countdown = server->game.time_elapsed / 1000;
			playerMessageBroadcast(0, MSG_SEND_GAME_START, SERVER_GAME_COUNTDOWN - server->game.countdown, 0, NULL);
		}
		return;
	}

	if (server->game.countdown < SERVER_GAME_COUNTDOWN) {
		playerMessageBroadcast(0, MSG_SEND_GAME_START, 0, 0, NULL);
		server->game.countdown = SERVER_GAME_COUNTDOWN;
		gameStart();
	}

	return;
}
