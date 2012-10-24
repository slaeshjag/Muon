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

	msg_buf->msg_buf_len = MESSAGE_BUFFER_INITIAL;
	msg_buf->msg_buf_use = 0;

	return msg_buf;
}


MESSAGE_BUFFER *messageBufferDelete(MESSAGE_BUFFER *msg_buf) {
	if (msg_buf == NULL)
		return NULL;

	free(msg_buf->message);
	free(msg_buf);
	return NULL;
}


int messageBufferGrow(MESSAGE_BUFFER *msg_buf) {
	MESSAGE *tmp;

	msg_buf->msg_buf_len += 128;
	if ((tmp = realloc(msg_buf->message, msg_buf->msg_buf_len)) == NULL) {
		msg_buf->msg_buf_len -= 128;
		return -1;
	}

	msg_buf->message = tmp;
	return 0;
}


int messageBufferPush(MESSAGE_BUFFER *msg_buf, MESSAGE *message) {
	if (msg_buf == NULL) {
		fprintf(stderr, "Message buffer is NULL, unable to push message\n");
		return -1;
	}

	if (msg_buf->msg_buf_use == msg_buf->msg_buf_len)
		if (messageBufferGrow(msg_buf) < 0) {
			fprintf(stderr, "Unable to grow message buffer, message was not pushed\n");
			return -1;
		}
	
	msg_buf->message[msg_buf->msg_buf_use] = *message;
	msg_buf->msg_buf_use++;

	return 0;
}


int messageBufferPop(MESSAGE_BUFFER *msg_buf, int msg_i, MESSAGE *message) {
	if (msg_buf == NULL) {
		fprintf(stderr, "Message buffer is NULL, unable to pop message %i\n", msg_i);
		return -1;
	}

	if (msg_i >= msg_buf->msg_buf_use)
		return -1;
	
	*message = msg_buf->message[msg_i];
	return msg_i + 1;
}


int messageBufferFlush(MESSAGE_BUFFER *msg_buf) {
	if (msg_buf == NULL)
		return -1;
	msg_buf->msg_buf_use = 0;

	return 0;
}
