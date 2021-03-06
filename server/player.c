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


PLAYER *playerInit(unsigned int players, int map_w, int map_h) {
	PLAYER *player;
	int i, j, err;

	if ((player = malloc(sizeof(PLAYER) * players)) == NULL)
		return NULL;
	err = 0;
	server->players = players;
	server->player = player;

	for (i = 0; i < players; i++) {
		if ((player[i].map = malloc(sizeof(PLAYER_MAP) * map_w * map_h)) == NULL)
			err = 1;
		else
			for (j = 0; j < map_w * map_h; j++) {
				player[i].map[j].power = 0;
				player[i].map[j].fog = 0;
			}
		
		player[i].status = PLAYER_UNUSED;
		player[i].msg_buf = messageBufferInit();
		player[i].socket = NULL;
		player[i].network.recv_stat = SERVER_PROCESS_NOTHING;
		player[i].network.send_stat = SERVER_PROCESS_NOTHING;
		player[i].last_ping_sent = 0;

		player[i].stats.buildings_raised = 0;
		player[i].stats.buildings_lost = 0;
		player[i].stats.buildings_destroyed = 0;
		player[i].stats.points = 0;
		player[i].stats.points_visible = 0;
		player[i].stats.buildtime = 0;
		player[i].stats.no_build_time = 0;

		controlpointInitPlayer(i);
		player[i].team = -1;
	}

	if (playerBuildQueueInit() < 0)
		err = 1;

	if (err) {
		for (i = 0; i < players; i++)
			free(player[i].map);
		errorPush(SERVER_ERROR_NO_MEMORY);
		server->players = 0;
		server->player = NULL;
		free(player);
		return NULL;
	}


	return server->player;
}


void playerKickAll() {
	int i;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status == PLAYER_UNUSED)
			continue;
		messageBufferPushDirect(i, i, MSG_SEND_KICKED, 0, 0, NULL);
	}

	return;
}



PLAYER *playerDestroy(PLAYER *player, int players) {
	int i;

	if (!player)
		return player;
	
	for (i = 0; i < players; i++) {
	//	if (server->player[i].status == PLAYER_UNUSED)
	//		continue;
		playerDisconnectKill(i);
		messageBufferDelete(server->player[i].msg_buf);
		free(player[i].map);
	}

	free(player);
	return NULL;
}


void playerBroadcastTeam(int team, unsigned int player, unsigned int command, unsigned int arg1, unsigned int arg2, void *data) {
	int i;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_LOBBY)
			continue;
		if (server->player[i].team != team)
			continue;
		messageBufferPushDirect(i, player, command, arg1, arg2, data);
	}

	return;
}


void playerMessageBroadcast(unsigned int player, unsigned int command, unsigned int arg1, unsigned int arg2, void *data) {
	MESSAGE msg;
	int i;

	msg.player_ID = player;
	msg.command = command;
	msg.arg[0] = arg1;
	msg.arg[1] = arg2;
	msg.extra = data;

	for (i = 0; i < server->players; i++)
		if (server->player[i].status > PLAYER_WAITING_FOR_IDENTIFY && server->player[i].status != PLAYER_BEING_DISCONNECTED)
			messageBufferPush(server->player[i].msg_buf, &msg);
	return;
}


void playerDisconnect(unsigned int player) {
	server->player[player].status = PLAYER_BEING_DISCONNECTED;
	server->player[player].last_ping_reply = time(NULL);

	return;
}


void playerDisconnectKill(unsigned int player) {
	int broadcast;
	
	broadcast = (server->player[player].status > PLAYER_WAITING_FOR_IDENTIFY) ? 1 : 0;
	if (server->player[player].status == PLAYER_UNUSED)
		return;

	/* We probably need to add a mutex here */
	if (server->game.started)
		unitRemove(server->player[player].spawn.x, server->player[player].spawn.y, player);
	server->player[player].status = PLAYER_UNUSED;
	messageBufferFlush(server->player[player].msg_buf);
	server->player[player].socket = networkSocketDisconnect(server->player[player].socket);

	if (broadcast)
		playerMessageBroadcast(player, MSG_SEND_LEAVE, 0, 0, NULL);
	fprintf(stderr, "Disconnecting player...\n");
	

	return;
}


void playerCheckIdentify() {
	int i;
	time_t now = time(NULL);

	for (i = 0; i < server->players; i++)
		if (server->player[i].status == PLAYER_WAITING_FOR_IDENTIFY && now - server->player[i].id_req_send >= PLAYER_ID_TIME) {
			messageSend(server->player[i].socket, i, MSG_SEND_KICKED, 0, 0, NULL);
			playerDisconnect(i);
		}
	return;
}


int playerSlot() {
	int i;

	for (i = 0; i < server->players; i++)
		if (!server->player[i].status)
			return i;
	return -1;
}


int playerCalcSetPower(unsigned int player, int x, int y, int mode) {
	int i, j, k, index, range, old, team;

	index = y * server->w + x;
	team = server->player[player].team;

	range = unitRange(server->map[index]->type);

	for (j = -1 * range; j <= range; j++) {
		if (x + j < 0 || x + j >= server->w)
			continue;
		for (k = -1 * range; k <= range; k++) {
			if (y + k < 0 || y + k >= server->h)
				continue;
			index = (y + k) * server->w + (x + j);
			if (j*j + k*k > range*range)
				continue;
			if (team > -1) {
				for (i = 0; i < server->players; i++) {
					if (server->player[i].status != PLAYER_IN_GAME_NOW)
						continue;
					if (server->player[i].team != team)
						continue;
					old = (server->player[i].map[index].power) ? 1 : 0;
					server->player[i].map[index].power += mode;
					if ((old && !server->player[i].map[index].power)
							|| (!old && server->player[i].map[index].power))
						messageBufferPushDirect(i, player, MSG_SEND_MAP_TILE_ATTRIB, 1 << (2 * (old)), index, NULL);

				}
			} else {
				old = (server->player[player].map[index].power) ? 1 : 0;
				server->player[player].map[index].power += mode;
				if ((old && !server->player[player].map[index].power)
						|| (!old && server->player[player].map[index].power))
					messageBufferPushDirect(player, player, MSG_SEND_MAP_TILE_ATTRIB, 1 << (2 * (old)), index, NULL);
			}
		}
	}

	return 0;
}


int playerCalcLOS(unsigned int player, int x, int y, int mode) {
	int i, j, k, los, index, building, owner, haz_los, t, team, oldfog, fogdiff, p;

	index = y * server->w + x;
	team = server->player[player].team;
	
	if (!server->map[index]) {
		mode = mode / abs(mode);
		los = unitRange(UNIT_DEF_RADAR);
	} else if (abs(mode) > 1) {
		los = abs(mode);
		mode = mode / abs(mode);
	} else if ((los = unitLOS(server->map[index]->type)) < 0)
		return 0;

	for (j = -1 * los; j <= los; j++) {
		if (x + j < 0 || x + j >= server->w)
			continue;
		for (k = -1 * los; k <= los; k++) {
			if (y + k < 0 || y + k >= server->h)
				continue;
			index = (y + k) * server->w + (x + j);
			haz_los = (j*j + k*k <= los*los) ? 1 : 0;
			building = (server->map[index]) ? server->map[index]->type : 0;
			owner = (server->map[index]) ? server->map[index]->owner : 0;
			if ((!building) && (server->map_c.tile_data[index] & 0xFFF) == UNIT_BUILDSITE) {
				building = UNIT_DEF_BUILDSITE_FREE;
			}
			
			p = server->map_c.tile_data[index] & 0x20000;

			if (team > -1)
				for (i = 0; i < server->players; i++) {
					if (server->player[i].status != PLAYER_IN_GAME_NOW)
						continue;
					if (server->player[i].team != team)
						continue;
					oldfog = (server->player[i].map[index].fog > 0);
					server->player[i].map[index].fog += haz_los * mode;
					t = (server->player[i].map[index].fog) ? 0 : 1;
					fogdiff = (server->player[i].map[index].fog > 0);
					fogdiff = (oldfog ^ fogdiff);
					if (fogdiff || (j == 0 && k == 0)) {
						if (server->map_c.tile_data[index] & 0x40000) 
							messageBufferPushDirect(i, i, MSG_SEND_MAP_TILE_ATTRIB, (p) ? 0x10 : 0x11, index, NULL);
						messageBufferPushDirect(i, i, MSG_SEND_MAP_TILE_ATTRIB, 1 << (1 + 2*(t)), index, NULL);
						if (mode > 0)
							unitAnnounce(owner, i, (!t) ? building : 0, index);
					}

						
				}
			else {
				oldfog = (server->player[player].map[index].fog > 0);
				server->player[player].map[index].fog += haz_los * mode;
				t = (server->player[player].map[index].fog) ? 0 : 1;
				fogdiff = (server->player[player].map[index].fog > 0);
				fogdiff = (oldfog ^ fogdiff);
				if (fogdiff || (j == 0 && k == 0)) {
					messageBufferPushDirect(player, player, MSG_SEND_MAP_TILE_ATTRIB, 1 << (1 + 2*(t)), index, NULL);
					if (mode > 0)
						unitAnnounce(owner, player, (!t) ? building : 0, index);
					if (server->map_c.tile_data[index] & 0x40000) 
						messageBufferPushDirect(player, player, MSG_SEND_MAP_TILE_ATTRIB, (p) ? 0x10 : 0x11, index, NULL);
				}
			}

		}
	}

	return 0;
}


int playerBuildQueueInit() {
	int i;


	for (i = 0; i < server->players; i++) {
		server->player[i].queue.queue.building = 0;
		server->player[i].queue.queue.time = 0;
		server->player[i].queue.queue.progress = 0;
		server->player[i].queue.queue.index = 0;
		server->player[i].queue.queue.in_use = 0;

		server->player[i].queue.queue.index = server->player[i].spawn.index;
		server->player[i].queue.queue.in_use = 1;
		server->player[i].queue.queue.ready = 0;
		server->player[i].buildspots = 0;
		server->player[i].buildspeed = 1.0f;
	}

	return 0;
}


int playerBuildQueueLoop(int msec) {
	int i, building, progress, unit;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME_NOW)
			continue;
		if (!server->player[i].queue.queue.building) {
			server->player[i].stats.no_build_time += msec;
			continue;
		}

		server->player[i].stats.buildtime += msec;
		unit = server->player[i].queue.queue.building;
		if (server->player[i].queue.queue.progress == 100 && !server->player[i].queue.queue.ready) {
			messageBufferPushDirect(i, i, MSG_SEND_UNIT_READY, unit, 1, NULL);
			server->player[i].queue.queue.ready = server->player[i].queue.queue.building;
			server->player[i].queue.queue.building = 0;
			continue;
		}

		building = server->player[i].queue.queue.building;
		server->player[i].queue.queue.time += msec * server->game.gamespeed * server->player[i].buildspeed;
		if (server->player[i].queue.queue.time > unit_buildtime[building])
			server->player[i].queue.queue.time = unit_buildtime[building];
		progress = server->player[i].queue.queue.time;
		progress *= 100;
		progress /= unit_buildtime[building];
		if (progress != server->player[i].queue.queue.progress)
			messageBufferPushDirect(i, i, MSG_SEND_BUILDING_PROGRESS, building, progress, NULL);
		server->player[i].queue.queue.progress = progress;
	}
	
	return 0;
}


int playerBuildQueueStart(int player, int building) {
	if (building <= UNIT_DEF_GENERATOR || building > UNITS_DEFINED)
		return -1;
	
	if (server->player[player].queue.queue.building > 0)
		return -1;
	server->player[player].queue.queue.building = building;
	server->player[player].queue.queue.progress = 0;
	server->player[player].queue.queue.time = 0;

	messageBufferPushDirect(player, player, MSG_SEND_BUILDING_PROGRESS, building, 0, NULL);
	return 0;
}


int playerBuildQueueUnitReady(int player, int building) {
	if (building <= UNIT_DEF_GENERATOR || building > UNITS_DEFINED)
		return -1;
	
	if (server->player[player].queue.queue.building != building)
		return -1;
	if (server->player[player].queue.queue.progress < 100)
		return -1;
	return 0;
}


int playerBuildQueueStop(int player, int building) {

	if (building <= UNIT_DEF_GENERATOR || building > UNITS_DEFINED)
		return -1;
	server->player[player].queue.queue.building = 0;
	server->player[player].queue.queue.ready = 0;

	return 0;
}


void playerClear(int player) {
	int i;

	messageBufferPushDirect(player, player, MSG_SEND_MAP_CLEAR, 0, 0, NULL);
	
	server->player[player].transfer = WORLD;
	server->player[player].transfer_pos = 0;
	
	for (i = 0; i < server->w * server->h; i++) {
		server->player[player].map[i].fog = 1;
/*		if (!server->map[i])
			continue;
		unitAnnounce(server->map[i]->owner, player, server->map[i]->type, i);*/
	}

	return;
}


void playerDefeatAnnounce(int player) {
	int i, team;

	team = server->player[player].team;
	if (server->player[player].team > -1) {
		for (i = 0; i < server->players; i++) {
			if (server->player[i].team == team && team > -1) {
				if (server->player[i].status == PLAYER_IN_GAME_NOW)
					return;
			} else if (team > -1)
				continue;
		}

		for (i = 0; i < server->players; i++)
			if (team == server->player[i].team)
				playerClear(i);
		return;
	}

	playerClear(player);

	return;
}


unsigned int playerCountPoints(int player) {
	int i, t;
	unsigned int total = 0;

	t = server->w * server->h;

	for (i = 0; i < t; i++) {
		if (server->player[player].map[i].power)
			total += unit_points[0];
		if (!server->map[i])
			continue;
		if (server->map[i]->owner == player)
			total += unit_points[server->map[i]->type];
	}

	return total;
}


void playerLoop() {
	int i;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_BEING_DISCONNECTED)
			continue;
		if (server->player[i].msg_buf->read_pos == server->player[i].msg_buf->write_pos)
			playerDisconnectKill(i);
	}

	return;
}
