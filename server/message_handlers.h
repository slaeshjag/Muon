#ifndef __MESSAGE_HANDLERS_H__
#define	__MESSAGE_HANDLERS_H__

#define	MESSAGE_HANDLERS	8

int messageHandlerInit();

typedef struct {
	void			(*handle[MESSAGE_HANDLERS])(unsigned int player, MESSAGE *message);
} MESSAGE_HANDLER;


#endif
