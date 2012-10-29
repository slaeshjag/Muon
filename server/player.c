#include "server.h"


PLAYER *playerInit(unsigned int players, int map_w, int map_h) {
	PLAYER *player;
	int i, j, err;

	if ((player = malloc(sizeof(PLAYER) * players)) == NULL)
		return NULL;
	err = 0;
	server->players = 0;
	server->player = NULL;

	for (i = 0; i < players; i++) {
		if ((player[i].map = malloc(sizeof(PLAYER_MAP) * map_w * map_h)) == NULL)
			err = 1;
		else
			for (j = 0; j < map_w * map_h; j++) {
				player[i].map[j].power = 0;
				player[i].map[j].fog = 1;
			}
		player[i].status = PLAYER_UNUSED;
		player[i].msg_buf = messageBufferInit();
		player[i].socket = NULL;
		player[i].process_recv = PLAYER_PROCESS_NOTHING;
	}

	if (err) {
		for (i = 0; i < players; i++)
			free(player[i].map);
		errorPush(SERVER_ERROR_NO_MEMORY);
		free(player);
		return NULL;
	}

	server->player = player;
	server->players = players;

	return server->player;
}


PLAYER *playerDestroy(PLAYER *player, int players) {
	int i;

	if (!player)
		return player;
	
	for (i = 0; i < players; i++) {
		playerDisconnect(i);
		messageBufferDelete(server->player[i].msg_buf);
		free(player[i].map);
	}

	free(player);
	return NULL;
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
		if (server->player[i].status > PLAYER_WAITING_FOR_IDENTIFY)
			messageBufferPush(server->player[i].msg_buf, &msg);
	return;
}


void playerDisconnect(unsigned int player) {
	int broadcast;

	broadcast = (server->player[player].status > PLAYER_WAITING_FOR_IDENTIFY) ? 1 : 0;

	/* We probably need to add a mutex here */
	gameKillPlayerUnits(player);
	server->player[player].status = PLAYER_UNUSED;
	messageBufferFlush(server->player[player].msg_buf);
	server->player[player].socket = networkSocketDisconnect(server->player[player].socket);

	fprintf(stderr, "Disconnecting player...\n");
	
	if (!broadcast)
		return;
	
	playerMessageBroadcast(player, MSG_SEND_LEAVE, 0, 0, NULL);

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


int playerCalcSetPower(unsigned int team, unsigned int player, int index, int mode) {
	int i, j, k, x, y, range, old;

	x = index % server->w;
	y = index / server->h;

	range = unitRange(server->map[index]->type);

	for (j = -1 * range; j <= range; j++) {
		if (x + j < 0 || x + j >= server->w)
			continue;
		for (k = -1 * range; k <= range; k++) {
			if (y + k < 0 || y + k >= server->h)
				continue;
			index = (y + k) * server->w + (x + j);
			if (team > -1) {
				for (i = 0; i < server->players; i++) {
					if (server->player[i].team != team)
						continue;
					old = (server->player[i].map[index].power) ? 1 : 0;
					server->player[i].map[index].power += mode;
					if ((old && !server->player[i].map[index].power)
							|| (!old && server->player[i].map[index].power))
						messageBufferPushDirect(i, player, MSG_SEND_MAP_TILE_ATTRIB, !old, index, NULL);

				}
			} else {
				old = (server->player[player].map[index].power) ? 1 : 0;
				server->player[player].map[index].power += mode;
				if ((old && !server->player[player].map[index].power)
						|| (!old && server->player[player].map[index].power))
					messageBufferPushDirect(player, player, MSG_SEND_MAP_TILE_ATTRIB, !old, index, NULL);
			}
		}
	}

	return 0;
}


int playerCalcLOS(unsigned int team, unsigned int player, int index, int mode) {
	int i, j, k, los, x, y, building, owner, haz_los, t;

	x = index % server->w;
	y = index / server->h;

	if ((los = unitLOS(server->map[index]->type)) == 0)
		return 0;

	for (j = -1 * los; j <= los; j++) {
		if (x + j < 0 || x + j >= server->w)
			continue;
		for (k = -1 * los; k <= los; k++) {
			if (y + k < 0 || y + k >= server->h)
				continue;
			index = (y + k) * server->w + (y + j);
			haz_los = (sqrtf(j*j + k*k) <= los) ? 1 : 0;
			building = (server->map[index]) ? server->map[index]->type : 0;
			owner = (server->map[index]) ? server->map[index]->owner : 0;

			if (team > -1)
				for (i = 0; i < server->players; i++) {
					if (server->player[i].team != team)
						continue;
					server->player[i].map[index].fog += haz_los * mode;
					t = (server->player[i].map[index].fog) ? 1 : 0;
					messageBufferPushDirect(i, i, MSG_SEND_MAP_TILE_ATTRIB, i << 1, 0, NULL);
					messageBufferPushDirect(i, owner, MSG_SEND_BUILDING_PLACE, (t) ? building : 0, index, NULL);
				}
			else {
				server->player[player].map[index].fog += haz_los * mode;
				t = (server->player[player].map[index].fog) ? 1 : 0;
				messageBufferPushDirect(player, player, MSG_SEND_MAP_TILE_ATTRIB, t << 1, index, NULL);
				messageBufferPushDirect(player, owner, MSG_SEND_BUILDING_PLACE, (t) ? building : 0, index, NULL);
			}
		}
	}

	return 0;
}
