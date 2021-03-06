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
#define	MSG_TILE_ATTRIB_TERRAIN_CLEAR	0x10
#define	MSG_TILE_ATTRIB_TERRAIN_SET	0x11

#define	MSG_BUILDING_STOP		0
#define	MSG_BUILDING_START		1
#define	MSG_BUILDING_HOLD		2

/* Message protocol, general stuff */

/* All messages are wrapped in a "chunk", which means before a group of messages, an int
   containing the number of bytes in the chunk (incl. this int).
   If the number of bytes isnt fully readable within a reasonable time, request a resend.
   If you got the data fine, you need to send a CHUNK_OK message, or you won't get any more data */

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

/* arg_1 is the shield value (0..100), arg_2 is index on the map that the building is at. */
#define	MSG_RECV_BUILDING_SHIELD	0x00C

/* arg_1 is the HP value (0..100), arg_2 is index on the map that the building is at. */
#define	MSG_RECV_BUILDING_HP		0x00D

/* arg_1 is the index on the map that the building is at, arg_2 is the index of the building attacking. If arg_1 and arg_2 is equal, attack is nothing. */
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

/* arg_1 is the unit ID, arg_2 is the number ready. No data. */
#define	MSG_RECV_UNIT_READY		0x015

/* arg_1 is the building type, arg_2 is the attack value in tiles. No data. */
#define	MSG_RECV_UNIT_ATTACK		0x016

/* arg_1 is the building type, arg_2 is the range value in tiles. No data. */
#define	MSG_RECV_UNIT_RANGE		0x017

/* arg_1 is the building type, arg_2 is the LoS value in tiles. No data */
#define	MSG_RECV_UNIT_LOS		0x018

/* arg_1 is the amounts of buildings contructed, arg_2 is the amount of buildings lost. No data. */
#define	MSG_RECV_PLAYER_STATS_1		0x019

/* arg_1 is the amount of buildings destroyed (by player,) arg_2 is the contruction time utilization efficiency */
#define	MSG_RECV_PLAYER_STATS_2		0x01A

/* arg_1 is nothing. arg_2 is tile flare is on */
#define	MSG_RECV_MAP_FLARE		0x01B

/* arg_1 is the controlpoint building ID, arg_2 is ready in percent. 100 is ready */
#define	MSG_RECV_CP_TIMER		0x01C

/* arg_1 is the controlpoint building type that was deployed, arg_2 is where the center of the deployment was */
#define	MSG_RECV_CP_DEPLOY		0x01D

/* arg_1 is the controlpoint building type that should have its timer removed. arg_2 is nothing. */
#define	MSG_RECV_CP_CLEAR		0x01E

/* arg_1 is nothing, arg_2 is tile ID of major impact */
#define	MSG_RECV_MAJOR_IMPACT		0x01F

/* arg_1 is the amount of points the player got, arg_2 is nothing. */
#define	MSG_RECV_PLAYER_POINTS		0x020


/* These commands have data after them */

/* player_id is always 0. arg_1 is the size of the map in bytes. The message has data (map file name.) */
#define	MSG_RECV_MAP_BEGIN		0x100

/* player_id is always 0. arg_1 is 0. The message has data (part of the map.) */
#define	MSG_RECV_MAP_CHUNK		0x101

/* arg_1 is the team it was sent to. This message has data (the chat message.) */
#define	MSG_RECV_CHAT			0x102

/* arg_1 is 0. This message has data (the players name.) */
#define	MSG_RECV_JOIN			0x103



/***** Commands the CLIENT can send ******/


/* No arguments. No data. Not used at the moment, but must be send when the server sends a PING. */
#define	MSG_SEND_PONG			0

/* arg_1 is the team to send to. 0 is everyone. Message must have data (the message that is send.) */
#define	MSG_SEND_CHAT			1


/* arg_1 is the player to kick. No data */
#define	MSG_SEND_KICK			2

/* arg_1 is the gamespeed. No data. 0 < gamespeed < 1001. 3 is default. */
#define	MSG_SEND_SET_GAMESPEED		3

/* No arguments. You must send this to get more messages */
#define	MSG_SEND_CHUNK_OK		4

/* No arguments. Send this to get the last chunk resent */
#define	MSG_SEND_CHUNK_RESEND		5

/* arg_1 is client version. Message must have data (player name, max. 31 bytes long. Send this only once.) */
#define	MSG_SEND_IDENTIFY		6

/* arg_1 is the team to be member of. 0 is no team. No data. */
#define	MSG_SEND_PLAYER_INFO		7

/* arg_1 is 1 if client is ready, 0 if not. arg_2 is download progress on the map (0..100) */
#define	MSG_SEND_READY			8

/* arg_1 is the building type to build. arg_2 is the action (0 = stop, 1 = start, 2 = hold) */
#define	MSG_SEND_START_BUILD		9

/* arg_1 is the building type to place, arg_2 is the index on the map to place it on */
#define	MSG_SEND_PLACE_BUILDING		10

/* arg_1 is map index the building to set attack from is at, arg_2 the tile index for the target building */
#define	MSG_SEND_SET_ATTACK		11

/* arg_1 is nothing, arg_2 is tile to set flare on */
#define	MSG_SEND_SET_FLARE		12

/* arg_1 is the controlpoint building type, arg_2 is the tile ID to center the deployment at */
#define	MSG_SEND_CP_DEPLOY		13


#endif
