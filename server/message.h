#ifndef __MESSAGE_H__
#define	__MESSAGE_H__

#define	MESSAGE_BUFFER_INITIAL		2048
#define	MESSAGE_MAX_PAYLOAD		512

#define	MESSAGE_ALWAYS_MAX		1

#define	MESSAGE_LOBBY_MIN		2
#define	MESSAGE_LOBBY_MAX		4

#define	MESSAGE_GAME_MIN		5
#define	MESSAGE_GAME_MAX		5

/* Messages we can always get */
#define	MSG_RECV_PONG			0
#define	MSG_RECV_CHAT			1

/* Messages we can only get in lobby mode */
#define	MSG_RECV_IDENTIFY		2
#define	MSG_RECV_MAP_PROGRESS		3
#define	MSG_RECV_READY			4

/* Messages we can only get in-game */

/* Messages we can send */
#define	MSG_SEND_SERVER_FULL		0
#define	MSG_SEND_REQUEST_IDENTIFY	1
#define	MSG_SEND_ILLEGAL_COMMAND	2
#define	MSG_SEND_BAD_CLIENT		3
#define	MSG_SEND_KICKED			4
#define	MSG_SEND_CHAT			5
#define	MSG_SEND_JOIN			6
#define	MSG_SEND_LEAVE			7
#define	MSG_SEND_MAP_BEGIN		8
#define	MSG_SEND_MAP_CHUNK		9
#define	MSG_SEND_MAP_END		10
#define	MSG_SEND_MAP_PROGRESS		11
#define	MSG_SEND_PING			12
#define	MSG_SEND_CLIENT_READY		13
#define	MSG_SEND_GAME_START		14


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
} MESSAGE_BUFFER;


MESSAGE_BUFFER *messageBufferInit();
MESSAGE_BUFFER *messageBufferDelete(MESSAGE_BUFFER *msg_buf);
int messageBufferPush(MESSAGE_BUFFER *msg_buf, MESSAGE *message);
int messageBufferPop(MESSAGE_BUFFER *msg_buf, MESSAGE *message);
int messageBufferFlush(MESSAGE_BUFFER *msg_buf);

int messageSend(SERVER_SOCKET *socket, unsigned int player, unsigned int message, int arg1, int arg2, void *data);
int messageExecute(unsigned int player, MESSAGE *message);
int messageHasData(unsigned int command, unsigned int arg);

#endif
