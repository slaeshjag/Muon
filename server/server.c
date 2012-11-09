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
	FILE *fp;
	int i, n, t, building, owner;


	if ((fp = fopen(path, "rb")) == NULL) {
		fprintf(stderr, "Unable to open map %s for cache", path);
		errorPush(SERVER_ERROR_CANT_OPEN_MAP);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	server->map_c.data_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if ((server->map_c.data = malloc(server->map_c.data_len)) == NULL) {		
		fprintf(stderr, "Unable to malloc for map cache\n");
		errorPush(SERVER_ERROR_NO_MEMORY);
		fclose(fp);
		return -1;
	}

	fread(server->map_c.data, server->map_c.data_len, 1, fp);
	fclose(fp);

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

	server->map_c.path = path;
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


int serverParseMapFirst() {
	int i, buildspots;

	buildspots = 1;		/* Player's generator counts! */
	for (i = 0; i < server->w * server->h; i++)
		if (server->map_c.tile_data[i] == UNIT_BUILDSITE)
			buildspots++;
	server->build_spots = buildspots;
	
	return 0;
}



/* TODO: Expand with arguments */
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
	serverParseMapFirst();
	
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


void serverProcessNetwork() {
	int i, t;
	MESSAGE msg, msg_c;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status == PLAYER_UNUSED)
			continue;
		if (server->player[i].process_recv) {
			msg_c = server->player[i].process_msg_recv;
			if ((t = networkReceiveTry(server->player[i].socket, msg_c.extra, msg_c.arg[1])) > 0) {
				server->player[i].process_recv = PLAYER_PROCESS_NOTHING;
				messageExecute(i, &msg_c);
			} else if (t == -1) {
				playerDisconnect(i);
				continue;
			} else
				continue;
		}

		while ((t = networkReceiveTry(server->player[i].socket, (void *) &msg, 16)) > 0) {
			msg.player_ID = ntohl(msg.player_ID);
			msg.command = ntohl(msg.command);
			msg.arg[0] = ntohl(msg.arg[0]);
			msg.arg[1] = ntohl(msg.arg[1]);

			if (messageHasData(msg.command, msg.arg[1]) == 0 && msg.arg[1]) {
				if (msg.arg[1] > MESSAGE_MAX_PAYLOAD) {
					messageSend(server->player[i].socket, i, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
					playerDisconnect(i);
					break;
				}
				
				if ((msg.extra = malloc(msg.arg[1] + 1)) == NULL) {
					fprintf(stderr, "Fatal error: System seems to be out of RAM. Server is dieing\n");
					errorPush(SERVER_ERROR_NO_MEMORY);
					serverStop();
					return;
				}

				if ((t = networkReceiveTry(server->player[i].socket, (void *) msg.extra, msg.arg[1])) > 0) {
					messageExecute(i, &msg);
					server->player[i].process_recv = PLAYER_PROCESS_NOTHING;
				} else {
					server->player[i].process_msg_recv = msg;
					server->player[i].process_recv = PLAYER_PROCESS_DATA;
					break;
				}
			} else {
				msg.extra = NULL;
				server->player[i].process_recv = PLAYER_PROCESS_NOTHING;
				messageExecute(i, &msg);
			}
		}

		if (t < 0)
			playerDisconnect(i);
	}


	for (i = 0; i < server->players; i++) {
		if (server->player[i].status == PLAYER_UNUSED)
			continue;
		if (server->player[i].process_send) {
			msg_c = server->player[i].process_msg_send;
			msg.player_ID = htonl(msg_c.player_ID);
			msg.command = htonl(msg_c.command);
			msg.arg[0] = htonl(msg_c.arg[0]);
			msg.arg[1] = htonl(msg_c.arg[1]);

			if (server->player[i].process_send == PLAYER_PROCESS_MSG) {
				if ((t = networkSend(server->player[i].socket, (void *) &msg, 16 - server->player[i].process_byte_send)) > 0) {
					if (t + server->player[i].process_byte_send < 16) {
						server->player[i].process_byte_send =+ t;
						continue;
					} else {
						server->player[i].process_send = (server->player[i].process_msg_send.extra) ? PLAYER_PROCESS_DATA : PLAYER_PROCESS_NOTHING;
						server->player[i].process_byte_send = 0;
					}
				} else if (t < 0) {
					playerDisconnect(i);
					continue;
				} else
					continue;
			}

			if (server->player[i].process_send == PLAYER_PROCESS_DATA) {
				if ((t = networkSend(server->player[i].socket, msg_c.extra, msg_c.arg[1])) > 0) {
					if (t + server->player[i].process_byte_send < msg_c.arg[1]) {
						server->player[i].process_byte_send += t;
						continue;
					} else {
						free(msg_c.extra);
						server->player[i].process_send = PLAYER_PROCESS_NOTHING;
					}
				} else if (t < 0) {
					playerDisconnect(i);
					continue;
				} else 
					continue;
			}
		}

		if (messageBufferPop(server->player[i].msg_buf, &msg_c) < 0)
			continue;
		msg.player_ID = htonl(msg_c.player_ID);
		msg.command = htonl(msg_c.command);
		msg.arg[0] = htonl(msg_c.arg[0]);
		msg.arg[1] = htonl(msg_c.arg[1]);
		while ((t = networkSend(server->player[i].socket, (void *) &msg, 16)) > 0) {
			server->player[i].process_send = PLAYER_PROCESS_DATA;
			if (msg_c.extra) {
				if ((t = networkSend(server->player[i].socket, (void *) msg_c.extra, msg_c.arg[1])) > 0) {
					if (t + server->player[i].process_byte_send < msg_c.arg[1]) {
						server->player[i].process_byte_send += t;
						break;
					} else {
						free(msg_c.extra);
						server->player[i].process_send = PLAYER_PROCESS_NOTHING;
					}
				} else if (t < 0) {
					playerDisconnect(i);
					break;
				} else {
					server->player[i].process_send = PLAYER_PROCESS_DATA;
					break;
				}
			} else
				server->player[i].process_send = PLAYER_PROCESS_NOTHING;
			if ((messageBufferPop(server->player[i].msg_buf, &msg_c)) < 0)
				break;
			msg.player_ID = htonl(msg_c.player_ID);
			msg.command = htonl(msg_c.command);
			msg.arg[0] = htonl(msg_c.arg[0]);
			msg.arg[1] = htonl(msg_c.arg[1]);
		}

		if (t == -1) {
			playerDisconnect(i);
			continue;
		}

	}
		

	return;
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
