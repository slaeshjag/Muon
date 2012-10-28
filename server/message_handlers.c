#include "server.h"


void messageHandlerIdentify(unsigned int player, MESSAGE *message) {
	int i;
	MESSAGE msg;

	if (message->arg[1] > PLAYER_NAME_LEN - 1) {
		messageSend(server->player[player].socket, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		playerDisconnect(player);
		free(message->extra);
		return;
	}

	if (message->arg[0] != SERVER_VERSION) {
		messageSend(server->player[player].socket, player, MSG_SEND_BAD_CLIENT, SERVER_VERSION, 0, NULL);
		playerDisconnect(player);
		free(message->extra);
		return;
	}

	memcpy(server->player[player].name, (char *) message->extra, message->arg[1]);
	server->player[player].name[message->arg[1]] = 0;
	server->player[player].status = PLAYER_IN_LOBBY;

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
			msg.command = MSG_SEND_MAP_PROGRESS;
			msg.arg[0] = server->player[i].map_progress;
			msg.arg[1] = 0;
			msg.extra = NULL;
			messageBufferPush(server->player[player].msg_buf, &msg);
			
			msg.player_ID = i;
			msg.command = MSG_SEND_CLIENT_READY;
			msg.arg[0] = (server->player[i].status == PLAYER_READY_TO_START) ? 1 : 0;
			msg.arg[1] = 0;
			msg.extra = NULL;
			messageBufferPush(server->player[player].msg_buf, &msg);
		}
	
	fprintf(stderr, "Debug: Player '%s' joined the game\n", server->player[player].name);

	lobbyMapSend(player);

	return;
}


void messageHandlerChat(unsigned int player, MESSAGE *message) {
	fprintf(stdout, "Debug: CHAT: <%s> ", server->player[player].name);
	fwrite(message, message->arg[1], 1, stdout);
	fprintf(stdout, "\n");

	playerMessageBroadcast(player, MSG_SEND_CHAT, 0, message->arg[1], message->extra);

	return;
}


void messageHandlerMapProgress(unsigned int player, MESSAGE *message) {
	playerMessageBroadcast(player, MSG_SEND_MAP_PROGRESS, message->arg[0], 0, NULL);
	server->player[player].map_progress = message->arg[0];

	if (message->arg[0] == 100)
		gameAttemptStart();

	return;
}


void messageHandlerPlayerReady(unsigned int player, MESSAGE *message) {
	playerMessageBroadcast(player, MSG_SEND_CLIENT_READY, message->arg[0], 0, NULL);

	if (!message->arg[0]) {
		server->player[player].status = PLAYER_IN_LOBBY;
		return;
	}

	server->player[player].status = PLAYER_READY_TO_START;
	gameAttemptStart();

	return;
}


void messageHandlerDummy(unsigned int player, MESSAGE *message) {
	fprintf(stderr, "MSG_RECV: %i  - Not implemented yet!\n", message->command);

	return;
}


int messageHandlerInit() {
	server->message_handler.handle[MSG_RECV_PONG] 		= messageHandlerDummy;
	server->message_handler.handle[MSG_RECV_CHAT] 		= messageHandlerChat;
	server->message_handler.handle[MSG_RECV_IDENTIFY] 	= messageHandlerIdentify;
	server->message_handler.handle[MSG_RECV_MAP_PROGRESS] 	= messageHandlerMapProgress;
	server->message_handler.handle[MSG_RECV_READY] 		= messageHandlerPlayerReady;
	server->message_handler.handle[5] 			= messageHandlerDummy;
	server->message_handler.handle[6] 			= messageHandlerDummy;
	server->message_handler.handle[7] 			= messageHandlerDummy;

	return 0;
}
