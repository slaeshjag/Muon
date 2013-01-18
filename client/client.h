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

#ifndef CLIENT_H
#define CLIENT_H

#define CLIENT_DEFAULT_PORT "1337"
#define CLIENT_DEFAULT_PLAYERS 2
#define CLIENT_DEFAULT_GAMESPEED 3
#define CLIENT_TIMEOUT 10000

#define MSG_SERVER_DISCONNECT 0x10000

typedef struct {
	unsigned int constructed;
	unsigned int lost;
	unsigned int destroyed;
	unsigned int efficiency;
	unsigned int score;
} STATS;

typedef struct {
	char name[32];
	STATS stats;
	unsigned int team;
	unsigned int ready:1;
} PLAYER;

static const char mapdir[]="maps";
static const unsigned char ldimagic[]={0x83, 0xB3, 0x66, 0x1B, 0xBB, 0xA7, 0x7A, 0xBC};

DARNIT_SOCKET *sock;
MESSAGE_RAW msg_recv;
void *msg_recv_offset;
unsigned char msg_recv_payload[512];
unsigned char *msg_recv_payload_offset;

int player_id;
//char *player_names;
PLAYER *player;
int players;

void (*client_message_handler)(MESSAGE_RAW *, unsigned char *);

void client_chat(int id, char *buf, int len);

int client_init(char *host, int port);
void client_disconnect(int cause);
void client_check_incoming();
void client_message_send(int player_id, int command, int arg_1, int arg_2, char *payload);

#endif
