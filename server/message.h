#ifndef __MESSAGE_H__
#define	__MESSAGE_H__

#define	MESSAGE_MAX		8
#define	MESSAGE_BUFFER_INITIAL	2048


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


#endif
