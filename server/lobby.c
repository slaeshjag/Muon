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


void lobbyMapSend(unsigned int player) {
	MESSAGE msg;
	int i, send;
	char *data = server->map_c.data;

	msg.player_ID = player;
	msg.command = MSG_SEND_MAP_BEGIN;
	msg.arg[0] = server->map_c.data_len;
	msg.arg[1] = strlen(server->map_c.path);
	msg.extra = (void *) server->map_c.path;
	messageBufferPush(server->player[player].msg_buf, &msg);
	
	for (i = 0; i < server->map_c.data_len; ) {
		send = (server->map_c.data_len - i > MESSAGE_MAX_PAYLOAD) ? MESSAGE_MAX_PAYLOAD : server->map_c.data_len - i;
		
		msg.player_ID = player;
		msg.command = MSG_SEND_MAP_CHUNK;
		msg.arg[0] = 0;
		msg.arg[1] = send;
		msg.extra = &data[i];
		messageBufferPush(server->player[player].msg_buf, &msg);
		i += send;
	}

	msg.player_ID = player;
	msg.command = MSG_SEND_MAP_END;
	msg.arg[0] = 0;
	msg.arg[1] = 0;
	msg.extra = NULL;
	messageBufferPush(server->player[player].msg_buf, &msg);

	return;
}



int lobbyPoll() {
	SERVER_SOCKET *socket;
	int slot;

	if ((socket = networkAccept(server->accept)) == NULL)
		return 0;
	if ((slot = playerSlot(server)) == -1) {
		messageSend(socket, 0, MSG_SEND_SERVER_FULL, 0, 0, NULL);
		networkSocketDisconnect(socket);
		return 0;
	}

	server->player[slot].status = PLAYER_WAITING_FOR_IDENTIFY;
	server->player[slot].socket = socket;
	server->player[slot].id_req_send = time(NULL);
	*server->player[slot].name = 0;
	server->player[slot].map_progress = 0;

	server->player[slot].process_recv = PLAYER_PROCESS_NOTHING;
	server->player[slot].process_send = PLAYER_PROCESS_NOTHING;
	server->player[slot].process_byte_send = 0;
	server->player[slot].last_ping_sent = 0;
	server->player[slot].team = -1;

	server->player[slot].stats.buildtime = 0;
	server->player[slot].stats.no_build_time = 0;
	server->player[slot].stats.buildings_raised = 0;
	server->player[slot].stats.buildings_lost = 0;

	messageSend(socket, slot, MSG_SEND_REQUEST_IDENTIFY, slot, server->players, NULL);

	return 0;
}
