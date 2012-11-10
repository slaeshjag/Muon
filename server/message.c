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


MESSAGE_BUFFER *messageBufferInit() {
	MESSAGE_BUFFER *msg_buf;

	if ((msg_buf = malloc(sizeof(MESSAGE_BUFFER))) == NULL) {
		fprintf(stderr, "Unable to malloc()\n");
		return NULL;
	}

	if ((msg_buf->message = malloc(sizeof(MESSAGE) * MESSAGE_BUFFER_INITIAL)) == NULL) {
		fprintf(stderr, "Unable to malloc()\n");
		free(msg_buf);
		return NULL;
	}

	msg_buf->len = MESSAGE_BUFFER_INITIAL;
	msg_buf->read_pos = msg_buf->write_pos = 0;

	return msg_buf;
}


MESSAGE_BUFFER *messageBufferDelete(MESSAGE_BUFFER *msg_buf) {
	if (msg_buf == NULL)
		return NULL;

	messageBufferFlush(msg_buf);

	free(msg_buf->message);
	free(msg_buf);
	return NULL;
}


int messageBufferPush(MESSAGE_BUFFER *msg_buf, MESSAGE *message) {
	if (msg_buf == NULL) {
		fprintf(stderr, "Message buffer is NULL, unable to push message\n");
		return -1;
	}

	if ((msg_buf->write_pos + 1 == msg_buf->len) ? 0 : msg_buf->write_pos + 1 == msg_buf->read_pos)
		return -1;
	
	msg_buf->message[msg_buf->write_pos] = *message;
	
	if (message->extra) {
		msg_buf->message[msg_buf->write_pos].extra  = malloc(message->arg[1]);
		memcpy(msg_buf->message[msg_buf->write_pos].extra, message->extra, message->arg[1]);
	}

	msg_buf->write_pos = (msg_buf->write_pos + 1 == msg_buf->len) ? 0 : msg_buf->write_pos + 1;

	return 0;
}


int messageBufferPushDirect(unsigned int to, unsigned int player, unsigned int message, unsigned int arg_1, unsigned int arg_2, void *data) {
	MESSAGE msg;

	msg.player_ID = player;
	msg.command = message;
	msg.arg[0] = arg_1;
	msg.arg[1] = arg_2;
	msg.extra = data;

	fprintf(stderr, "To: %i, from: %i. Packet %i (%i %i)\n", to, player, message, arg_1, arg_2);

	return messageBufferPush(server->player[to].msg_buf, &msg);
}



int messageBufferPop(MESSAGE_BUFFER *msg_buf, MESSAGE *message) {
	if (msg_buf == NULL) {
		fprintf(stderr, "Message buffer is NULL, unable to pop message\n");
		return -1;
	}

	if (msg_buf->read_pos == msg_buf->write_pos) 
		return -1;
	
	*message = msg_buf->message[msg_buf->read_pos];
	msg_buf->read_pos++;
	if (msg_buf->read_pos == msg_buf->len)
		msg_buf->read_pos = 0;
	return 0;
}


int messageBufferFlush(MESSAGE_BUFFER *msg_buf) {
	MESSAGE msg;
	if (msg_buf == NULL)
		return -1;
	
	while (messageBufferPop(msg_buf, &msg) > -1)
		free(msg.extra);

	return 0;
}


int messageSend(SERVER_SOCKET *socket, unsigned int player, unsigned int message, int arg1, int arg2, void *data) {
	int i, t;
	MESSAGE msg;
	char *buf;

	msg.player_ID = ntohl(player);
	msg.command = ntohl(message);
	msg.arg[0] = ntohl(arg1);
	msg.arg[1] = ntohl(arg2);
	buf = (char *) &msg;

	for (i = t = 0; i < 16 && t > -1; ) {
		t = networkSend(socket, &buf[i], 16 - i);
		i += t;
	}
	
	if (data) {
		buf = data;
		for (i = t = 0; i < arg2 && t > -1; ) {
			t = networkSend(socket, &buf[i], arg2 - i);
			i += t;
		}
	}

	return 0;
}


int messageExecute(unsigned int player, MESSAGE *message) {

	if (message->command >= MESSAGE_HANDLERS) {
		messageSend(server->player[player].socket, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		free(message->extra);
		return -1;				/* Illegal command */
	}

	if (server->player[player].status == PLAYER_WAITING_FOR_IDENTIFY && message->command != MSG_RECV_IDENTIFY) {
		messageSend(server->player[player].socket, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		messageSend(server->player[player].socket, player, MSG_SEND_KICKED, 0, 0, NULL);
		free(message->extra);
		
		playerDisconnect(player);
		return -1;
	}

	if ((!server->game.started) && message->command > MESSAGE_LOBBY_MAX) {
		messageSend(server->player[player].socket, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		free(message->extra);
		return -1;
	}	

	if (server->game.started && (message->command < MESSAGE_GAME_MIN || message->command > MESSAGE_GAME_MAX) && message->command > MESSAGE_ALWAYS_MAX) {
		messageSend(server->player[player].socket, player, MSG_SEND_ILLEGAL_COMMAND, 0, 0, NULL);
		free(message->extra);
		return -1;
	}

	(server->message_handler.handle[message->command])(player, message);
	free(message->extra);

	return 0;
}


int messageHasData(unsigned int command, unsigned int arg) {
	switch (command) {
		case MSG_RECV_CHAT:
		case MSG_RECV_IDENTIFY:
			return 0;
		case MSG_RECV_PONG:
		case MSG_RECV_READY:
		default:
			return -1;
	}

	return -1;
}
