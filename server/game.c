/* Muon - a new kind of rts
 * Copyright 2012 Steven Arnow <s@rdw.se> and Axel Isaksson (mr3d/h4xxel)
 * 
 * This file is part of Muon.
 * 
 * Muon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Muon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Muon.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "server.h"


int gameInit(int gamespeed) {
	server->game.started = 0;
	server->game.time_elapsed = 0;
	server->game.countdown = 0;

	/* FIXME: Make this variable */
	server->game.gamespeed = gamespeed;

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

		server->player[i].status = PLAYER_IN_GAME_NOW;
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
			if (owner < 0 || owner >= server->players)
				continue;
			if (server->player[owner].status != PLAYER_IN_GAME_NOW) 
				continue;
			if (building >= UNITS_DEFINED)
				continue;

			unitSpawn(owner, building, i % server->w, i / server->w);
		}
	
	unitPylonPulse();
	
	return;
}


void gameStart() {
	int i;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_GAME_NOW)
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


int gameDetectIfOver() {
	int i, team;
	
	team = -2;
	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME_NOW)
			continue;
		if (team == -1)
			return -1;
		if (server->player[i].team > -1) {
			if (server->player[i].team != team)
				return -1;
		}
		
		team = server->player[i].team;
	}

	return 0;
}


void gameEnd() {
	int i, team, player;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME_NOW)
			continue;
		if (server->player[i].status == PLAYER_IN_GAME_NOW) {
			team = server->player[i].team;
			player = i;
			server->player[i].status = PLAYER_SPECTATING;
			playerClear(i);
		}
	}

	team += 1;
	playerMessageBroadcast(player, MSG_SEND_GAME_ENDED, player, team, NULL);

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_SPECTATING)
			continue;
		fprintf(stdout, "=== Stats for %s (ID: %i) ===\n", server->player[i].name, i);
		fprintf(stdout, "Buildings raised: 	%i\n", server->player[i].stats.buildings_raised);
		fprintf(stdout, "Buildings lost:	%i\n", server->player[i].stats.buildings_lost);
		fprintf(stdout, "Buildings destroyed:	%i\n", server->player[i].stats.buildings_destroyed);
		fprintf(stderr, "Buildtime efficiency:	%i\n", server->player[i].stats.buildtime*100 / (server->player[i].stats.buildtime + server->player[i].stats.no_build_time));
		fprintf(stdout, "\n\n");
	}

	return;
}
