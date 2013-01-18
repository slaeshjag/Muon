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

#ifndef __MESSAGE_H__
#define	__MESSAGE_H__

#define	MESSAGE_BUFFER_INITIAL		65536
#define	MESSAGE_MAX_PAYLOAD		512
#define	MESSAGE_SIZE			16
#define	MESSAGE_SEND_BUFFER_SIZE	4096

#define	MESSAGE_ALWAYS_MAX		5

#define	MESSAGE_LOBBY_MIN		6
#define	MESSAGE_LOBBY_MAX		8

#define	MESSAGE_GAME_MIN		9
#define	MESSAGE_GAME_MAX		13

/* Messages we can always get */
#define	MSG_RECV_PONG			0
#define	MSG_RECV_CHAT			1
#define	MSG_RECV_KICK			2
#define	MSG_RECV_SET_GAMESPEED		3
#define	MSG_RECV_CHUNK_OK		4
#define	MSG_RECV_CHUNK_RESEND		5

/* Messages we can only get in lobby mode */
#define	MSG_RECV_IDENTIFY		6
#define	MSG_RECV_MAP_PROGRESS		7
#define	MSG_RECV_READY			8

/* Messages we can only get in-game */
#define	MSG_RECV_START_BUILD		9
#define	MSG_RECV_PLACE_BUILDING		10
#define	MSG_RECV_SET_ATTACK		11
#define	MSG_RECV_SET_FLARE		12
#define	MSG_RECV_CP_DEPLOY		13

/* Messages we can send */

#define	MSG_SEND_MAP_BEGIN		0x100
#define	MSG_SEND_MAP_CHUNK		0x101
#define	MSG_SEND_CHAT			0x102
#define	MSG_SEND_JOIN			0x103

#define	MSG_SEND_SERVER_FULL		0x000
#define	MSG_SEND_REQUEST_IDENTIFY	0x001
#define	MSG_SEND_ILLEGAL_COMMAND	0x002
#define	MSG_SEND_BAD_CLIENT		0x003
#define	MSG_SEND_KICKED			0x004
#define	MSG_SEND_LEAVE			0x005
#define	MSG_SEND_MAP_END		0x006
#define	MSG_SEND_PLAYER_INFO		0x007
#define	MSG_SEND_PING			0x008
#define	MSG_SEND_CLIENT_READY		0x009
#define	MSG_SEND_GAME_START		0x00A
#define	MSG_SEND_BUILDING_PLACE		0x00B
#define	MSG_SEND_BUILDING_SHIELD	0x00C
#define	MSG_SEND_BUILDING_HP		0x00D
#define	MSG_SEND_BUILDING_ATTACKING	0x00E
#define	MSG_SEND_BUILDING_PROGRESS	0x00F
#define	MSG_SEND_MAP_TILE_ATTRIB	0x010
#define	MSG_SEND_PLAYER_DEFEATED	0x011
#define	MSG_SEND_NAME_IN_USE		0x012
#define	MSG_SEND_MAP_CLEAR		0x013
#define	MSG_SEND_GAME_ENDED		0x014
#define	MSG_SEND_UNIT_READY		0x015
#define	MSG_SEND_UNIT_ATTACK		0x016
#define	MSG_SEND_UNIT_RANGE		0x017
#define	MSG_SEND_UNIT_LOS		0x018
#define	MSG_SEND_PLAYER_STATS_1		0x019
#define	MSG_SEND_PLAYER_STATS_2		0x01A
#define	MSG_SEND_MAP_FLARE		0x01B
#define	MSG_SEND_CP_DELAY		0x01C
#define	MSG_SEND_CP_DEPLOY		0x01D
#define	MSG_SEND_CP_CLEAR		0x01E
#define	MSG_SEND_MAJOR_IMPACT		0x01F

typedef struct {
	unsigned int		player_ID;
	unsigned int		command;
	unsigned int		arg[2];
	void			*extra;
} MESSAGE;


typedef struct {
	MESSAGE			*message;
	unsigned int		len;
	unsigned int		read_pos;
	unsigned int		write_pos;
	unsigned char		*send_buff;
	unsigned int		send_buff_size;
} MESSAGE_BUFFER;


MESSAGE_BUFFER *messageBufferInit();
MESSAGE_BUFFER *messageBufferDelete(MESSAGE_BUFFER *msg_buf);
int messageBufferPush(MESSAGE_BUFFER *msg_buf, MESSAGE *message);
int messageBufferPop(MESSAGE_BUFFER *msg_buf, MESSAGE *message);
int messageBufferGetNextSize(MESSAGE_BUFFER *msg_buf);
int messageBufferFlush(MESSAGE_BUFFER *msg_buf);
int messageBufferPushDirect(unsigned int to, unsigned int player, unsigned int message, unsigned int arg_1, unsigned int arg_2, void *data);


int messageSend(SERVER_SOCKET *socket, unsigned int player, unsigned int message, int arg1, int arg2, void *data);
int messageExecute(unsigned int player, MESSAGE *message);
int messageHasData(unsigned int command, unsigned int arg);

#endif
