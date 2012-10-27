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
		return;
	
	msg_buf->message[msg_buf->write_pos] = *message;
	msg_buf->write_pos++;
	if (msg_buf->write_pos == msg_buf->len)
		msg_buf->write_pos = 0;

	return 0;
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
	if (msg_buf == NULL)
		return -1;
	msg_buf->write_pos = msg_buf->read_pos = 0;

	return 0;
}


int messageSend(SERVER_SOCKET *socket, unsigned int player, unsigned int message, int arg1, int arg2, void *data) {
	int i;
	MESSAGE msg;
	char *buf;

	msg.player_ID = ntohl(player);
	msg.command = ntohl(message);
	msg.arg[0] = ntohl(arg1);
	msg.arg[1] = ntohl(arg2);
	buf = (char *) &msg;
	for (i = 0; i < 16; )
		i += networkSend(socket, &buf[i], 16 - i);
	
	return 0;
}
