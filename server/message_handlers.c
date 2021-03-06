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
			msg.extra = server->player[i].name;
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
	server->player[player].transfer = MAP;
	server->player[player].transfer_pos = 0;
	messageBufferPushDirect(player, player, MSG_SEND_MAP_BEGIN, server->map_c.data_len, strlen(server->map_c.path), (char *) server->map_c.path);

	server->player[player].last_ping_reply = time(NULL);

	return;
}


void messageHandlerChat(unsigned int player, MESSAGE *message) {
	if (server->player[player].status == PLAYER_SPECTATING) {
		messageBufferPushDirect(player, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		return;
	}
	
	if (!message->arg[0])
		playerMessageBroadcast(player, MSG_SEND_CHAT, 0, message->arg[1], message->extra);
	else
		playerBroadcastTeam((int) message->arg[0] - 1, player, MSG_SEND_CHAT, message->arg[0], message->arg[1], message->extra);

	return;
}


void messageHandlerPlayerInfo(unsigned int player, MESSAGE *message) {
	int i;

	if (server->player[player].status >= PLAYER_READY_TO_START)
		return;
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

	if (server->game.countdown < SERVER_GAME_COUNTDOWN && server->game.started) {
		fprintf(stderr, "Aborting countdown\n");
		server->game.started = 0;
		server->game.time_elapsed = 0;
	}

	if (!message->arg[0]) {
		server->player[player].status = PLAYER_IN_LOBBY;
		return;
	}

	server->player[player].status = PLAYER_READY_TO_START;
	gameAttemptStart();

	return;
}


void messageHandlerPong(unsigned int player, MESSAGE *message) {
	if (server->player[player].status == PLAYER_BEING_DISCONNECTED)
		return;
	server->player[player].last_ping_reply = time(NULL);
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
		unitDestroy(player, message->arg[1], player);
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
	if (server->player[player].queue.queue.ready != message->arg[0])/* Pff. lol. */
		return;
	if (unitValidateWall(message->arg[1], player) < 0)		/* D: */
		return;
	if (server->map_c.tile_data[message->arg[1]] & 0x10000)		/* Nope.jpg */
		return;
	
	/* Congratulations great commander, you have passed the seven trials! */
	/* Now, your building is being placed and you can enjoy the havoc it may cause */
	unitSpawn(player, message->arg[0], message->arg[1] % server->w, message->arg[1] / server->w);

	return;
}


void messageHandlerDummy(unsigned int player, MESSAGE *message) {
	fprintf(stderr, "MSG_RECV: %i  - Not implemented yet!\n", message->command);

	return;
}


void messageHandlerKick(unsigned int player, MESSAGE *message) {
	if (player != server->server_admin) {
		messageBufferPushDirect(player, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		return;
	}

	if (message->arg[0] >= server->players)
		return;
	messageSend(server->player[message->arg[0]].socket, message->arg[0], MSG_SEND_KICKED, 0, 0, NULL);
	playerDisconnect(message->arg[0]);

	return;
}


void messageHandlerSetAttack(unsigned int player, MESSAGE *message) {
	if (unitAttackValidate(message->arg[0], message->player_ID, message->arg[1]) < 0)
		return;
	unitAttackSet(message->arg[0], message->arg[1]);
	
	return;
}


void messageHandlerSetFlare(unsigned int player, MESSAGE *message) {
	int i, team;
	if (message->arg[1] >= server->w * server->h) {
		messageBufferPushDirect(player, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		return;
	}
	
	team = server->player[player].team;
	
	for (i = 0; i < server->players; i++) {
		if (server->player[i].status < PLAYER_IN_GAME_NOW)
			continue;
		if (server->player[i].team == -1 && i != player)
			continue;
		if (server->player[i].team != team)
			continue;
		messageBufferPushDirect(i, player, MSG_SEND_MAP_FLARE, 0, message->arg[1], NULL);
	}

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


void messageHandlerChunkOK(unsigned int player, MESSAGE *message) {
	server->player[player].network.ready_to_send = 1;

	return;
}


void messageHandlerChunkResend(unsigned int player, MESSAGE *message) {
	serverResend(player);

	return;
}


void messageHandlerControlpointDeploy(unsigned int player, MESSAGE *message) {
	controlpointDeploy(player, message->arg[0], message->arg[1]);

	return;
}


int messageHandlerInit() {
	server->message_handler.handle[MSG_RECV_PONG] 		= messageHandlerPong;
	server->message_handler.handle[MSG_RECV_CHAT] 		= messageHandlerChat;
	server->message_handler.handle[MSG_RECV_KICK] 		= messageHandlerKick;
	server->message_handler.handle[MSG_RECV_SET_GAMESPEED]	= messageHandlerSetGamespeed;
	server->message_handler.handle[MSG_RECV_CHUNK_OK]	= messageHandlerChunkOK;
	server->message_handler.handle[MSG_RECV_CHUNK_RESEND]	= messageHandlerChunkResend;

	server->message_handler.handle[MSG_RECV_IDENTIFY] 	= messageHandlerIdentify;
	server->message_handler.handle[MSG_RECV_MAP_PROGRESS] 	= messageHandlerPlayerInfo;
	server->message_handler.handle[MSG_RECV_READY] 		= messageHandlerPlayerReady;
	server->message_handler.handle[MSG_RECV_START_BUILD]	= messageHandlerStartBuild;
	server->message_handler.handle[MSG_RECV_PLACE_BUILDING]	= messageHandlerPlaceBuilding;
	server->message_handler.handle[MSG_RECV_SET_ATTACK]	= messageHandlerSetAttack;
	server->message_handler.handle[MSG_RECV_SET_FLARE]	= messageHandlerSetFlare;
	server->message_handler.handle[MSG_RECV_CP_DEPLOY]	= messageHandlerControlpointDeploy;

	return 0;
}
