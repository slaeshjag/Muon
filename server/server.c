#include "server.h"


int serverInitMap(const char *path) {
	FILE *fp;
	int i, j, x, y;
	char buff[32];


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

	for (i = 0; i < server->players; i++) {
		sprintf(buff, "player_%i", i + 1);
		sscanf(ldmzFindProp(server->map_data, buff), "%i, %i", &server->player[i].spawn.x, &server->player[i].spawn.y);
		server->player[i].spawn.index = server->player[i].spawn.y * server->w + server->player[i].spawn.x;
	}

	for (i = 0; i < server->players; i++) {
		x = server->player[i].spawn.x;
		y = server->player[i].spawn.y;
		for (j = 0; j < server->players; j++) {
			if (j == i)
				continue;
			if (x == server->player[j].spawn.x && y == server->player[j].spawn.y) {
				fprintf(stderr, "Overlapping spawn points is not allowed: Bad map!\n");
				errorPush(SERVER_ERROR_SPAWNS_OVERLAP);
				free(server->map_c.data);
				server->map_c.data = NULL;
				return -1;
			}
		}
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


/* TODO: Expand with arguments */
SERVER *serverStart(const char *fname, unsigned int players, int port) {
	int i, map_w, map_h;
	const char *tmp;

	if (server)
		return server;
	
	errorInit();

	if ((server = malloc(sizeof(SERVER))) == NULL) {
		fprintf(stderr, "Unable to allocate a server\n");
		errorPush(SERVER_ERROR_NO_MEMORY);
		return NULL;
	}

	messageHandlerInit(server);
	gameInit();

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
	server->w = map_w;
	server->h = map_h;
	if (serverInitMap(fname) < 0)
		server = serverStop();
	if (atoi(tmp) < players) {
		fprintf(stderr, "Map file is only defined for %i players (%i player slots requested)\n", atoi(tmp), players);
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

			if (messageHasData(msg.command, msg.arg[1]) == 0) {
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
	} else 
		gameLoop(d_ms);

	serverSendPing();
	
	/* This call must ALWAYS be LAST in serverLoop */
	serverProcessNetwork();

	return 0;
}

