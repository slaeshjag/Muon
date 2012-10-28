#ifndef __MESSAGE_H__
#define	__MESSAGE_H__

/* Message protocol, general stuff */

/* This struct is used for *all* message bodies */
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
#define	MSG_RECV_SERVER_FULL		0

/* arg_1 is your player ID. Requires you to send an identify message */
#define	MSG_RECV_REQUEST_IDENTIFY	1

/* No arguments. Not very helpful, but if you get it before you are accepted into the lobby, it is fatal */
#define	MSG_RECV_ILLEGAL_COMMAND	2

/* arg_1 is the accepted client version on the server. This message is fatal */
#define	MSG_RECV_BAD_CLIENT		3

/* arg_1 may be used for reason in the future, but for now you can ignore all arguments */
#define	MSG_RECV_KICKED			4

/* arg_1 is 0. This message has data (the chat message.) */
#define	MSG_RECV_CHAT			5

/* arg_1 is 0. This message has data (the players name.) */
#define	MSG_RECV_JOIN			6

/* Both arguments are 0 */
#define	MSG_RECV_LEAVE			7

/* player_id is always 0. arg_1 is the size of the map in bytes. The message has data (map file name.) */
#define	MSG_RECV_MAP_BEGIN		8

/* player_id is always 0. arg_1 is 0. The message has data (part of the map.) */
#define	MSG_RECV_MAP_CHUNK		9

/* player_id is always 0. No arguments, but arg_1 and arg_2 may be used for checksum in the future. */
#define	MSG_RECV_MAP_END		10

/* arg_1 is in the range 0..100. Used to display map transfer progress for players in lobby. No data. */
#define	MSG_RECV_MAP_PROGRESS		11

/* No arguments. No data. Not used at the moment, but is reserved for future use. Client must reply with PONG */
#define	MSG_RECV_PING			12

/* arg_1 is 1 if the player is ready, 0 if not. No data. */
#define	MSG_RECV_PLAYER_READY		13

/* No arguments. No data. When you get this message, you should switch from lobby to game play mode */
#define	MSG_RECV_GAME_START		14


/***** Commands the CLIENT can send ******/


/* No arguments. No data. Not used at the moment, but must be send when the server sends a PING. */
#define	MSG_SEND_PONG			0

/* arg_1 is ignored. Message must have data (the message that is send.) */
#define	MSG_SEND_CHAT			1

/* arg_1 is client version. Message must have data (player name, max. 31 bytes long. Send this only once.) */
#define	MSG_SEND_IDENTIFY		2

/* arg_1 is progress in range 0..100. No data. 100 means map downloaded and loaded OK. */
#define	MSG_SEND_MAP_PROGRESS		3

/* arg_1 is 1 if client is ready, 0 if not. No data. */
#define	MSG_SEND_READY			4



#endif
