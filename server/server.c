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


int serverInitMap(const char *path) {
	int i, n, t, building, owner;


	if (!(server->map_c.data = ldmzCache(path, &server->map_c.data_len))) {
		fprintf(stderr, "Unable to malloc for map cache\n");
		errorPush(SERVER_ERROR_NO_MEMORY);
		return -1;
	}

	for (i = 0; i < server->players; i++)
		server->player[i].spawn.x = server->player[i].spawn.y = -1;
	n = server->w * server->h;
	for (i = 0; i < n; i++)
		if (server->map_c.tile_data[i] != 0) {
			t = server->map_c.tile_data[i] & 0xFFF;
			building = (t % 8) + 1;
			owner = (t / 8) - 1;
			if (owner < 0 || owner >= server->players)
				continue;
			if (building == UNIT_DEF_GENERATOR) {
				server->player[owner].spawn.x = i % server->w;
				server->player[owner].spawn.y = i / server->w;
				server->player[owner].spawn.index = i;
			}

		}

	for (i = 0; i < server->players; i++)
		if (server->player[i].spawn.x == -1) {
			fprintf(stderr, "Map is missing spawn point for player %i\n", i);
			errorPush(SERVER_ERROR_SPAWN_MISSING);
			free(server->map_c.data);
			server->map_c.data = NULL;
			return -1;
		}


	for (;;) {
		if (strstr(path, "/") != NULL)
			path = strstr(path, "/") + 1;
		else
			break;
	}

	for (;;) {
		if (strstr(path, "\\") != NULL)
			path = strstr(path, "\\") + 1;
		else
			break;
	}

	for (server->map_c.path = path; path; server->map_c.path = path) {
		if (strstr(path, "/") == NULL)
			break;
		path = strstr(path, "/") + 1;
	}
	
	path = server->map_c.path;
	if (!(server->map_c.path = malloc(strlen(path) + 1)))
		return -1;
	strcpy((char *) server->map_c.path, path);

	server->pylons = NULL;

	return 0;
}


int serverPowerGet(int owner, int x, int y) {
	return server->player[owner].map[x + y * server->w].power;
}


void serverInit() {
	server = NULL;

	errorInit();
	
	return;
}


int serverIsRunning() {
	return (server) ? 0 : -1;
}


SERVER *serverStart(const char *fname, unsigned int players, int port, int gamespeed) {
	int i, map_w, map_h;
	const char *tmp;

	if (server)
		return server;
	
	if (gamespeed < 0) {
		errorPush(SERVER_ERROR_GAMESPEED_TOO_SMALL);
		return NULL;
	}

	errorInit();

	if ((server = malloc(sizeof(SERVER))) == NULL) {
		fprintf(stderr, "Unable to allocate a server\n");
		errorPush(SERVER_ERROR_NO_MEMORY);
		return NULL;
	}

	server->map_c.path = NULL;
	messageHandlerInit(server);
	gameInit(gamespeed);

	if ((server->map_data = ldmzLoad(fname)) == NULL) {
		free(server);
		server = NULL;
		fprintf(stderr, "Unable to load map %s\n", fname);
		errorPush(SERVER_ERROR_INVALID_MAP);
		return NULL;
	}

	if (strcmp((tmp = ldmzFindProp(server->map_data, "max_players")), "NO SUCH KEY") == 0) {
		tmp = NULL;
		fprintf(stderr, "Missing map property max_players in mapfile\n");
		errorPush(SERVER_ERROR_MAP_NO_MAX_PLAYERS);
	}

	ldmzGetSize(server->map_data, &map_w, &map_h);
	server->map = malloc(sizeof(SERVER_UNIT *) * map_w * map_h);
	server->map_c.tile_data = ldmzGetData(server->map_data);
	server->w = map_w;
	server->h = map_h;
	
	playerInit(players, map_w, map_h);
	server->accept = networkListen(port);

	if (!server->map || !server->player || !server->accept || !tmp) {
		if (!server->map)
			errorPush(SERVER_ERROR_NO_MEMORY);
		if (!server->accept)
			errorPush(SERVER_ERROR_UNABLE_TO_LISTEN);
		fprintf(stderr, "Unable to allocate a server\n");
		playerDestroy(server->player, server->players);
		networkSocketDisconnect(server->accept);
		ldmzFree(server->map_data);
		free(server);
		server = NULL;
		return NULL;
	}

	for (i = 0; i < map_w * map_h; i++)
		server->map[i] = NULL;

	server->unit = NULL;
	if (serverInitMap(fname) < 0) {
		server = serverStop();
		return server;
	}

	if (atoi(tmp) < players) {
		fprintf(stderr, "Map file is only defined for %i players (%i player slots requested) %s\n", atoi(tmp), players, tmp);
		errorPush(SERVER_ERROR_TOO_MANY_PLAYERS);
		server = serverStop();
	}

	return server;
}


SERVER *serverStop() {
	int i;
	
	if (!server)
		return NULL;

	for (i = 0; i < server->w * server->h; i++) {
		free(server->map[i]);
	}
	
	playerDestroy(server->player, server->players);
	free(server->map);
	networkSocketDisconnect(server->accept);
	ldmzFree(server->map_data);
	free(server->map_c.data);
	free((void *) server->map_c.path);

	free(server);
	server = NULL;

	return NULL;
}


void serverSendPing() {
	time_t now;
	int i;
	MESSAGE msg;

	msg.command = MSG_SEND_PING;
	msg.arg[0] = msg.arg[1] = 0;

	now = time(NULL);

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_LOBBY)
			continue;

		if (now - server->player[i].last_ping_sent >= SERVER_PING_INTERVAL) {
			msg.command = MSG_SEND_PING;
			msg.arg[0] = msg.arg[1] = 0;
			msg.player_ID = i;
			msg.extra = NULL;
			messageBufferPush(server->player[i].msg_buf, &msg);
			server->player[i].last_ping_sent = now;
		}
	}

	return;
}


int serverRecv(PLAYER_NETWORK *network, int player) {
	int t;

	if (network->recv_stat == SERVER_PROCESS_NOTHING) {
		network->recv_stat = SERVER_PROCESS_MSG;
		network->recv_pos = 0;
	}

	if (network->recv_stat == SERVER_PROCESS_MSG) {
		t = networkReceiveTry(server->player[player].socket, &((char *) &network->recv)[network->recv_pos], MESSAGE_SIZE - network->recv_pos);
		if (t < 0)
			return SERVER_PROCESS_FAIL;
		if (t == 0)
			return SERVER_PROCESS_INCOMPLETE;
		network->recv_pos += t;
		if (network->recv_pos < MESSAGE_SIZE)
			return SERVER_PROCESS_INCOMPLETE;

		network->recv.player_ID = ntohl(network->recv.player_ID);
		network->recv.command = ntohl(network->recv.command);
		network->recv.arg[0] = ntohl(network->recv.arg[0]);
		network->recv.arg[1] = ntohl(network->recv.arg[1]);
		network->recv.extra = (messageHasData(network->recv.command, network->recv.arg[1]) == 0) ? malloc(network->recv.arg[1]) : NULL;

		if (!network->recv.extra) {
			network->recv_stat = SERVER_PROCESS_NOTHING;
			return SERVER_PROCESS_DONE;
		}
		
		network->recv_stat = SERVER_PROCESS_DATA;
		network->recv_pos = 0;
	}

	if (network->recv_stat == SERVER_PROCESS_DATA) {
		t = networkReceiveTry(server->player[player].socket, network->recv.extra, network->recv.arg[1] - network->recv_pos);
		if (t < 0) {
			free(network->recv.extra);
			network->recv.extra = NULL;
			return SERVER_PROCESS_FAIL;
		} else if (t == 0)
			return SERVER_PROCESS_INCOMPLETE;
		
		network->recv_pos += t;
		if (network->recv_pos < network->recv.arg[1])
			return SERVER_PROCESS_INCOMPLETE;
		network->recv_stat = SERVER_PROCESS_NOTHING;
		return SERVER_PROCESS_DONE;
	}

	return SERVER_PROCESS_FAIL;
}


void serverResend(int player) {
	PLAYER_NETWORK *network = &server->player[player].network;

	network->send_stat = SERVER_PROCESS_MSG;
	network->send_pos = 0;
	network->ready_to_send = 1;

	return;
}



int serverSend(PLAYER_NETWORK *network, MESSAGE_BUFFER *buffer, int player) {
	int t;
	MESSAGE msg;

	if (network->send_stat == SERVER_PROCESS_NOTHING) {
		
		t = buffer->send_buff_size = 4;
		for (;;) {
			if (t + messageBufferGetNextSize(buffer) > MESSAGE_SEND_BUFFER_SIZE)
				break;
			if (messageBufferGetNextSize(buffer) < 0) {
				if (serverRequestMoreData(player) < 0) {
					break;
				} else {
					continue;
				}
			}


			messageBufferPop(buffer, &network->send);
			msg.player_ID = htonl(network->send.player_ID);
			msg.command = htonl(network->send.command);
			msg.arg[0] = htonl(network->send.arg[0]);
			msg.arg[1] = htonl(network->send.arg[1]);
	
			network->send_stat = SERVER_PROCESS_MSG;
			memcpy(&buffer->send_buff[t], (void *) &msg, MESSAGE_SIZE);
			t += MESSAGE_SIZE;

			if (network->send.extra) {
				memcpy(&buffer->send_buff[t], network->send.extra, network->send.arg[1]);
				t += network->send.arg[1];
			}

			free(network->send.extra);
			
		}
		
		if (t == 4)
			return SERVER_PROCESS_INCOMPLETE;
		
		buffer->send_buff_size = t;
		network->send_pos = 0;
		((int *) buffer->send_buff)[0] = htonl(t);
	}

	if (network->send_stat == SERVER_PROCESS_MSG) {

		t = networkSend(server->player[player].socket, (void *) &buffer->send_buff[network->send_pos], buffer->send_buff_size - network->send_pos);

		if (t < 0) {
			return SERVER_PROCESS_FAIL;
		}

		network->send_pos += t;
		if (network->send_pos < buffer->send_buff_size)
			return SERVER_PROCESS_INCOMPLETE;
		
		network->send_stat = SERVER_PROCESS_NOTHING;
		server->player[player].network.ready_to_send = 0;
		
		return SERVER_PROCESS_DONE;
	}

	return SERVER_PROCESS_FAIL;
}



void serverProcessNetwork() {
	int i, t;
	PLAYER_NETWORK *network;

	/* Recieve */
	for (i = 0; i < server->players; i++) {
		if (server->player[i].status == PLAYER_UNUSED)
			continue;
		network = &server->player[i].network;
		
		while ((t = serverRecv(network, i)) > 0) {
			messageExecute(i, &server->player[i].network.recv);
		}
		
		if (t == SERVER_PROCESS_INCOMPLETE)
			continue;
		playerDisconnect(i);
	}

	/* Send */
	for (i = 0; i < server->players; i++) {
		if (server->player[i].status == PLAYER_UNUSED)
			continue;

		if (!server->player[i].network.ready_to_send)
			continue;

		t = serverSend(network, server->player[i].msg_buf, i);
		
		if (t == SERVER_PROCESS_INCOMPLETE)
			continue;
		if (t > 0)
			continue;
		playerDisconnect(i);
	}
		

	return;
}


/* Pushes a message to the message buffer, when there's room in the chunk but no more data to send */
int serverRequestMoreData(unsigned int player) {
	switch (server->player[player].transfer) {
		case MAP:
			return lobbyMapSend(player);
		case WORLD:
			return gameWorldTransfer(player);
		default:
			return -1;
	}

	return -1;
}


int serverLoop(unsigned int d_ms) {
	/* FIXME: STUB */
	if (!server)
		return -1;

	if (!server->game.started) {
		lobbyPoll();
		playerCheckIdentify();
	} else {
		playerBuildQueueLoop(d_ms);
		unitLoop(d_ms);
		gameLoop(d_ms);
	}

	serverSendPing();
	
	/* This call must ALWAYS be LAST in serverLoop */
	serverProcessNetwork();

	return 0;
}


int serverPlayersPlaying() {
	int i, p;

	if (!server)
		return 0;
	for (i = p = 0; i < server->players; i++)
		if (server->player[i].status == PLAYER_IN_GAME_NOW)
			p++;
	return p;
}


void serverAdminSet(int player) {
	if (!server)
		return;
	server->server_admin = player;

	return;
}
