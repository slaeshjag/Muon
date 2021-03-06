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
	int n, i, t, j, building, owner;

	n = server->w * server->h;
	for (j = 0; j < server->players; j++) {
		for (i = 0; i < n; i++)
			if (server->map_c.tile_data[i]) {
				t = server->map_c.tile_data[i] & 0xFFF;
				if (!t)
					continue;
				building = (t % MAP_BUILDINGS_PER_PLAYER) + 1;
				owner = (t / MAP_BUILDINGS_PER_PLAYER) - 1;
				if (owner != server->player[j].spawn_as)
					continue;
				if (server->player[j].status != PLAYER_IN_GAME_NOW)
					continue;
				if (building >= UNITS_DEFINED)
					continue;
				
				if (building == UNIT_DEF_GENERATOR) {
					server->player[j].spawn.x = i % server->w;
					server->player[j].spawn.y = i / server->w;
					server->player[j].spawn.index = i;
				}
				
				if (building >= UNIT_DEF_BUILDSITE)
					server->map_c.tile_data[i] = UNIT_BUILDSITE;
				unitSpawn(j, building, i % server->w, i / server->w);
			}
		server->player[j].stats.points = 0;
		server->player[j].stats.points_visible = 0;
		server->player[j].stats.buildings_raised = 0;
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


int gameWorldTransfer(unsigned int player) {
	int i;

	for (i = server->player[player].transfer_pos; i < server->w * server->h; i++) {
		server->player[player].map[i].fog = 1;
		if (server->map_c.tile_data[i] & 0x40000)
			messageBufferPushDirect(player, player, MSG_SEND_MAP_TILE_ATTRIB, (server->map_c.tile_data[i] & 0x20000) ? 0x11 : 0x10, i, NULL);
		if (!server->map[i]) {
			if ((server->map_c.tile_data[i] & 0xFFF) == UNIT_BUILDSITE)
				unitAnnounce(player, player, UNIT_DEF_BUILDSITE_FREE, i);
			else if (server->map_c.tile_data[i] & 0x80000)
				unitAnnounce(player, player, 0, i);
		}

		else
			unitAnnounce(server->map[i]->owner, player, server->map[i]->type, i);
		server->player[player].transfer_pos = i + 1;

		return 0;
	}

	server->player[player].transfer = NONE;
	server->player[player].transfer_pos = i;
	
	return -1;
}


int gameDetectIfOver() {
	int i, team;
	
	team = -2;
	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME_NOW)
			continue;
		if (team == -1)
			return -1;
		if (team > -1) {
			if (server->player[i].team != team)
				return -1;
		}
		
		team = server->player[i].team;
	}

	return 0;
}


void gameEnd() {
	int i, team, player, eff, points;
	PLAYER_STATS stats;

	team = -1;
	player = 0;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_GAME_NOW)
			continue;
		if (server->player[i].status == PLAYER_SPECTATING) {
			playerClear(i);
			continue;
		}

		if (server->player[i].status == PLAYER_IN_GAME_NOW) {
			team = server->player[i].team;
			player = i;
			server->player[i].status = PLAYER_SPECTATING;
			playerClear(i);
		}
	}

	team += 1;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_SPECTATING)
			continue;
		stats = server->player[i].stats;
		eff = stats.buildtime * 100 / (stats.buildtime + stats.no_build_time);
		points = server->player[i].stats.points_visible;
		points = (points < 0) ? 0 : points;

		playerMessageBroadcast(i, MSG_SEND_PLAYER_STATS_1, stats.buildings_raised, stats.buildings_lost, NULL);
		playerMessageBroadcast(i, MSG_SEND_PLAYER_STATS_2, stats.buildings_destroyed, eff, NULL);
		playerMessageBroadcast(i, MSG_SEND_PLAYER_POINTS, points, 0, NULL);
		fprintf(stderr, "Total amount of points for %.31s: %.8i\n", server->player[i].name, points);
	}
	
	playerMessageBroadcast(player, MSG_SEND_GAME_ENDED, team, player, NULL);

	return;
}
