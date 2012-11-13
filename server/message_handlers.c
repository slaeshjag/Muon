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


void messageHandlerIdentify(unsigned int player, MESSAGE *message) {
	int i;
	MESSAGE msg;

	if (message->arg[1] > PLAYER_NAME_LEN - 1) {
		messageSend(server->player[player].socket, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		playerDisconnect(player);
		return;
	}

	if (message->arg[0] != SERVER_VERSION) {
		messageSend(server->player[player].socket, player, MSG_SEND_BAD_CLIENT, SERVER_VERSION, 0, NULL);
		playerDisconnect(player);
		return;
	}


	memcpy(server->player[player].name, (char *) message->extra, message->arg[1]);
	server->player[player].name[message->arg[1]] = 0;
	for (i = 0; i < server->players; i++)
		if (server->player[i].status >= PLAYER_IN_LOBBY)
			if (strcmp(server->player[i].name, server->player[player].name) == 0) {
				messageSend(server->player[player].socket, player, MSG_SEND_NAME_IN_USE, 0, 0, NULL);
				playerDisconnect(player);
				return;
			}

	server->player[player].status = PLAYER_IN_LOBBY;
	i = (player == server->server_admin) ? 1 : 0;
	messageBufferPushDirect(player, player, MSG_SEND_PLAYER_INFO, 0, 0, NULL);
	playerMessageBroadcast(player, MSG_SEND_JOIN, 0, strlen(server->player[player].name), server->player[player].name);

	for (i = 0; i < server->players; i++)
		if (server->player[i].status >= PLAYER_IN_LOBBY && i != player) {
			msg.player_ID = i;
			msg.command = MSG_SEND_JOIN;
			msg.arg[0] = 0;
			msg.arg[1] = strlen(server->player[i].name);
			msg.extra = strdup(server->player[i].name);
			messageBufferPush(server->player[player].msg_buf, &msg);

			msg.player_ID = i;
			msg.command = MSG_SEND_CLIENT_READY;
			msg.arg[0] = (server->player[i].status == PLAYER_READY_TO_START) ? 1 : 0;
			msg.arg[1] = server->player[i].map_progress;
			msg.extra = NULL;
			messageBufferPush(server->player[player].msg_buf, &msg);

			messageBufferPushDirect(player, i, MSG_SEND_PLAYER_INFO, server->player[i].team + 1, 0, NULL);
		}

	unitAnnounceBuildingData(player);
	lobbyMapSend(player);

	return;
}


void messageHandlerChat(unsigned int player, MESSAGE *message) {
	if (server->player[player].status == PLAYER_SPECTATING) {
		messageBufferPushDirect(player, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		return;
	}

	playerMessageBroadcast(player, MSG_SEND_CHAT, 0, message->arg[1], message->extra);

	return;
}


void messageHandlerPlayerInfo(unsigned int player, MESSAGE *message) {
	int i;

	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_LOBBY)
			continue;
		if (server->server_admin == i)
			messageBufferPushDirect(i, player, MSG_SEND_PLAYER_INFO, message->arg[0], 1, NULL);
		else
			messageBufferPushDirect(i, player, MSG_SEND_PLAYER_INFO, message->arg[0], 0, NULL);
	}
	
	server->player[player].team = message->arg[0] - 1;

	return;
}


void messageHandlerPlayerReady(unsigned int player, MESSAGE *message) {
	playerMessageBroadcast(player, MSG_SEND_CLIENT_READY, message->arg[0], message->arg[1], NULL);
	server->player[player].map_progress = message->arg[1];

	if (!message->arg[0]) {
		server->player[player].status = PLAYER_IN_LOBBY;
		return;
	}

	server->player[player].status = PLAYER_READY_TO_START;
	gameAttemptStart();

	return;
}


void messageHandlerPong(unsigned int player, MESSAGE *message) {
	/* TODO: Implement */
	return;
}


void messageHandlerStartBuild(unsigned int player, MESSAGE *message) {
	if (message->arg[1])
		playerBuildQueueStart(player, message->arg[0]);
	else
		playerBuildQueueStop(player, message->arg[0]);
	
	return;
}


void messageHandlerPlaceBuilding(unsigned int player, MESSAGE *message) {
	if (message->arg[0] == 0) {
		unitDestroy(player, message->arg[1]);
		return;
	}

	if (message->arg[1] > server->w * server->h)			/* Meh, nice try :P */
		return;
	if (!server->player[player].map[message->arg[1]].power)		/* Sorry! :x */
		return;
	if (!server->player[player].map[message->arg[1]].fog)		/* Nope. */
		return;
	if (server->map[message->arg[1]])				/* Even more nope. */
		return;
	if (!server->player[player].queue.ready[message->arg[0]].count)	/* Pff. lol. */
		return;
	if (unitValidateWall(message->arg[1], player) < 0)		/* D: */
		return;
	
	/* Congratulations great commander, you have passed the six trials! */
	/* Now, your building is being placed and you can enjoy the havoc it may cause */
	unitSpawn(player, message->arg[0], message->arg[1] % server->w, message->arg[1] / server->w);

	return;
}


void messageHandlerDummy(unsigned int player, MESSAGE *message) {
	fprintf(stderr, "MSG_RECV: %i  - Not implemented yet!\n", message->command);

	return;
}


void messageHandlerKick(unsigned int player, MESSAGE *message) {
	if (player != server->server_admin)
		messageBufferPushDirect(player, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
	if (message->arg[0] >= server->players)
		return;
	messageBufferPushDirect(message->arg[0], player, MSG_SEND_KICKED, 0, 0, NULL);
	playerDisconnect(message->arg[0]);

	return;
}


void messageHandlerSetAttack(unsigned int player, MESSAGE *message) {
	if (unitAttackValidate(message->arg[0], message->player_ID, message->arg[1]) < 0)
		return;
	unitAttackSet(message->arg[0], message->arg[1]);
	
	return;
}


void messageHandlerSetGamespeed(unsigned int player, MESSAGE *message) {
	if (message->arg[0] == 0)
		return;
	if (message->arg[0] > 1000)	/* 1000 gamespeed should be enough for everyone */
		return;
	server->game.gamespeed = message->arg[0];

	return;
}


int messageHandlerInit() {
	server->message_handler.handle[MSG_RECV_PONG] 		= messageHandlerPong;
	server->message_handler.handle[MSG_RECV_CHAT] 		= messageHandlerChat;
	server->message_handler.handle[MSG_RECV_KICK] 		= messageHandlerKick;
	server->message_handler.handle[MSG_RECV_SET_GAMESPEED]	= messageHandlerSetGamespeed;

	server->message_handler.handle[MSG_RECV_IDENTIFY] 	= messageHandlerIdentify;
	server->message_handler.handle[MSG_RECV_MAP_PROGRESS] 	= messageHandlerPlayerInfo;
	server->message_handler.handle[MSG_RECV_READY] 		= messageHandlerPlayerReady;
	server->message_handler.handle[MSG_RECV_START_BUILD]	= messageHandlerStartBuild;
	server->message_handler.handle[MSG_RECV_PLACE_BUILDING]	= messageHandlerPlaceBuilding;
	server->message_handler.handle[MSG_RECV_SET_ATTACK]	= messageHandlerSetAttack;

	return 0;
}
