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
				player[i].map[j].fog = 0;
			}
		player[i].status = PLAYER_UNUSED;
		player[i].msg_buf = messageBufferInit();
		player[i].socket = NULL;
		player[i].process_recv = PLAYER_PROCESS_NOTHING;
	}
	
	server->players = players;
	server->player = player;
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

	playerBuildQueueDestroy();
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
					if (server->player[i].team != team)
						continue;
					if (server->player[i].status  != PLAYER_IN_GAME)
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
	int i, j, k, los, index, building, owner, haz_los, t, team, oldfog, fogdiff;

	index = y * server->w + x;
	team = server->player[player].team;

	if ((los = unitLOS(server->map[index]->type)) == 0)
		return 0;

	fprintf(stderr, "Calculating LoS\n");
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
			if (!building && server->map_c.tile_data[index] == UNIT_BUILDSITE)
				building = UNIT_DEF_BUILDSITE_FREE;

			if (team > -1)
				for (i = 0; i < server->players; i++) {
					if (server->player[i].team != team)
						continue;
					if (server->player[i].status != PLAYER_IN_GAME)
						continue;
					oldfog = (server->player[player].map[index].fog > 0);
					server->player[player].map[index].fog += haz_los * mode;
					t = (server->player[player].map[index].fog) ? 0 : 1;
					fogdiff = (server->player[player].map[index].fog > 0);
					fogdiff = (oldfog ^ fogdiff);
					if (((t && mode == -1) || (!t && mode == 1)) || (j == 0 && k == 0)) {
						messageBufferPushDirect(i, i, MSG_SEND_MAP_TILE_ATTRIB, 1 << (1 + 2*(t)), 0, NULL);
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
					unitAnnounce(owner, player, (!t) ? building : 0, index);
				}
			}

		}
	}

	return 0;
}


int playerBuildQueueInit() {
	int i, j, err;

	err = 0;

	for (i = 0; i < server->players; i++)
		if (!(server->player[i].queue.queue = malloc(sizeof(PLAYER_BUILDQUEUE_E) * server->build_spots)))
			err = 1;
	if (err) {
		for (i = 0; i < server->players; i++)
			free(server->player[i].queue.queue);
		errorPush(SERVER_ERROR_NO_MEMORY);

		return -1;
	}

	for (i = 0; i < server->players; i++) {
		for (j = 0; j < server->build_spots; j++) {
			server->player[i].queue.queue[j].building = 0;
			server->player[i].queue.queue[j].time = 0;
			server->player[i].queue.queue[j].progress = 0;
			server->player[i].queue.queue[j].index = 0;
			server->player[i].queue.queue[j].in_use = 0;
		}

		server->player[i].queue.queue[0].index = server->player[i].spawn.index;
		server->player[i].queue.queue[0].in_use = 1;
	}

	return 0;
}


int playerBuildQueueDestroy() {
	int i;

	for (i = 0; i < server->players; i++)
		free(server->player[i].queue.queue);
	return 0;
}


int playerBuildQueueLoop(int msec) {
	int i, j, building, progress;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status != PLAYER_IN_GAME)
			continue;
		for (j = 0; j < server->build_spots; j++) {
			if (!server->player[i].queue.queue[j].in_use)
				continue;
			if (!server->player[i].queue.queue[j].building)
				continue;
			if (server->player[i].queue.queue[j].progress == 100)
				continue;
			building = server->player[i].queue.queue[j].building;
			server->player[i].queue.queue[j].time += msec * server->game.gamespeed;
			if (server->player[i].queue.queue[j].time > unit_buildtime[building])
				server->player[i].queue.queue[j].time = unit_buildtime[building];
			progress = server->player[i].queue.queue[j].time;
			progress *= 100;
			progress /= unit_buildtime[building];
			if (progress != server->player[i].queue.queue[j].progress)
				messageBufferPushDirect(i, i, MSG_SEND_BUILDING_PROGRESS, building, progress, NULL);
			server->player[i].queue.queue[j].progress = progress;
		}
	}
	
	return 0;
}


int playerBuildQueueStart(int player, int building) {
	int i;

	if (building <= UNIT_DEF_GENERATOR || building > UNITS_DEFINED)
		return -1;

	for (i = 0; i < server->build_spots; i++) {
		if (!server->player[player].queue.queue[i].in_use)
			continue;
		if (server->player[player].queue.queue[i].building)
			continue;
		server->player[player].queue.queue[i].building = building;
		server->player[player].queue.queue[i].time = 0;
		server->player[player].queue.queue[i].progress = 0;
		messageBufferPushDirect(player, player, MSG_SEND_BUILDING_PROGRESS, building, 0, NULL);
		return 0;
	}

	return -1;
}


int playerBuildQueueUnitReady(int player, int building) {
	int i;

	if (building <= UNIT_DEF_GENERATOR || building > UNITS_DEFINED)
		return -1;

	for (i = 0; i < server->build_spots; i++) {
		if (server->player[player].queue.queue[i].building != building)
			continue;
		if (server->player[player].queue.queue[i].progress < 100)
			return -1;
		return 0;
	}

	return -1;
}


int playerBuildQueueStop(int player, int building) {
	int i;

	if (building <= UNIT_DEF_GENERATOR || building > UNITS_DEFINED)
		return -1;
	for (i = 0; i < server->build_spots; i++) {
		if (!server->player[player].queue.queue[i].in_use)
			continue;
		if (server->player[player].queue.queue[i].building == building) {
			server->player[player].queue.queue[i].building = 0;
			server->player[player].queue.queue[i].time = 0;
			server->player[player].queue.queue[i].progress = 0;
			return 0;
		}
	}

	return -1;
}
