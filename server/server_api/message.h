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

#define	MSG_PAYLOAD_BIT			0x100

#define	MSG_TILE_ATTRIB_POWER_SET	0x1
#define	MSG_TILE_ATTRIB_FOW_CLEAR	0x2
#define	MSG_TILE_ATTRIB_POWER_CLEAR	0x4
#define	MSG_TILE_ATTRIB_FOW_SET		0x8

#define	MSG_BUILDING_STOP		0
#define	MSG_BUILDING_START		1
#define	MSG_BUILDING_HOLD		2

/* Message protocol, general stuff */

/* This struct is used for *all* message bodies. Ints are in correct (big) endian */
typedef struct {
	/* When sending messages, this field is ignored */
	/* The server uses this field to tell the client what player caused the message */
	unsigned int			player_id;

	/* When sending a message to the server, use a command with the MSG_SEND prefix */
	/* When getting a message, commands with the MSG_RECV prefix are used to decode it */
	unsigned int			command;

	/* Generic argument. Can be zero. */
	unsigned int			arg_1;

	/* Generic argument. If the message has additional data, this field tells you how many bytes. */
	/* When a message has data, the data immediately follows the message */
	/* If the message has no data, and this field isn't used, it is normally 0. */
	unsigned int			arg_2;
} MESSAGE_RAW;

/* NOTE: When strings are sent, they are *NOT* NULL-terminated */



/***** Commands the SERVER can send *****/


/* No arguments, no data. The message is fatal. */
#define	MSG_RECV_SERVER_FULL		0x000

/* arg_1 is your player ID. arg_2 is the maximum amount of player on the map. Requires you to send an identify message */
#define	MSG_RECV_REQUEST_IDENTIFY	0x001

/* No arguments. Not very helpful, but if you get it before you are accepted into the lobby, it is fatal */
#define	MSG_RECV_ILLEGAL_COMMAND	0x002

/* arg_1 is the accepted client version on the server. This message is fatal */
#define	MSG_RECV_BAD_CLIENT		0x003

/* arg_1 may be used for reason in the future, but for now you can ignore all arguments */
#define	MSG_RECV_KICKED			0x004

/* Both arguments are 0 */
#define	MSG_RECV_LEAVE			0x005

/* player_id is always 0. No arguments, but arg_1 and arg_2 may be used for checksum in the future. */
#define	MSG_RECV_MAP_END		0x006

/* arg_1 is the team the player is member of. 0 is no team. arg_2 is 1 if the player is admin. No data. */
#define	MSG_RECV_PLAYER_INFO		0x007

/* No arguments. No data. Not used at the moment, but is reserved for future use. Client must reply with PONG */
#define	MSG_RECV_PING			0x008

/* arg_1 is 1 if the player is ready, 0 if not. arg_2 is map download progress. (0..100). No data. */
#define	MSG_RECV_PLAYER_READY		0x009

/* arg_1 is the number of seconds left until the game starts. 0 means the game has started. No data. */
#define	MSG_RECV_GAME_START		0x00A

/* arg_1 is building type, arg_2 is index on the map (x + y * map_w). */
#define	MSG_RECV_BUILDING_PLACE		0x00B

/* arg_1 is the shield value (0..1000), arg_2 is index on the map that the building is at. */
#define	MSG_RECV_BUILDING_SHIELD	0x00C

/* arg_1 is the HP value (0..1000), arg_2 is index on the map that the building is at. */
#define	MSG_RECV_BUILDING_HP		0x00D

/* arg_1 is the index on the map that the building is at, arg_2 is the index of the building attacking */
#define	MSG_RECV_BUILDING_ATTACK	0x00E

/* arg_1 is the building type that is being built, arg_2 is the progress in the range 0..100 */
#define	MSG_RECV_BUILDING_PROGRESS	0x00F

/* arg_1 is the attribute bitmask for that tile, arg_2 is the index on the map */
#define	MSG_RECV_MAP_TILE_ATTRIB	0x010

/* No arguemnts. No data. */
#define	MSG_RECV_PLAYER_DEFEATED	0x011

/* No arguments. No data. */
#define	MSG_RECV_NAME_IN_USE		0x012

/* No arguments, no data. When the client gets this, it's okay to clear all FoW */
#define	MSG_RECV_CLEAR_MAP		0x013


/* arg_1 is team. If team is 0, arg_2 is the winning player. */
#define	MSG_RECV_GAME_ENDED		0x014


/* These commands actually have data after them */

/* player_id is always 0. arg_1 is the size of the map in bytes. The message has data (map file name.) */
#define	MSG_RECV_MAP_BEGIN		0x100

/* player_id is always 0. arg_1 is 0. The message has data (part of the map.) */
#define	MSG_RECV_MAP_CHUNK		0x101

/* arg_1 is 0. This message has data (the chat message.) */
#define	MSG_RECV_CHAT			0x102

/* arg_1 is 0. This message has data (the players name.) */
#define	MSG_RECV_JOIN			0x103



/***** Commands the CLIENT can send ******/


/* No arguments. No data. Not used at the moment, but must be send when the server sends a PING. */
#define	MSG_SEND_PONG			0

/* arg_1 is ignored. Message must have data (the message that is send.) */
#define	MSG_SEND_CHAT			1


/* arg_1 is the player to kick. No data */
#define	MSG_SEND_KICK			2

/* arg_1 is the gamespeed. No data. 0 < gamespeed < 1001. 3 is default. */
#define	MSG_SEND_SET_GAMESPEED		3

/* arg_1 is client version. Message must have data (player name, max. 31 bytes long. Send this only once.) */
#define	MSG_SEND_IDENTIFY		4

/* arg_1 is the team to be member of. 0 is no team. No data. */
#define	MSG_SEND_PLAYER_INFO		5

/* arg_1 is 1 if client is ready, 0 if not. arg_2 is download progress on the map (0..100) */
#define	MSG_SEND_READY			6

/* arg_1 is the building type to build. arg_2 is the action (0 = stop, 1 = start, 2 = hold) */
#define	MSG_SEND_START_BUILD		7

/* arg_1 is the building type to place, arg_2 is the index on the map to place it on */
#define	MSG_SEND_PLACE_BUILDING		8

/* arg_1 is map index the building to set attack from is at, arg_2 the tile index for the target building */
#define	MSG_SEND_SET_ATTACK		9


#endif
