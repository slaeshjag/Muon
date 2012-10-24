#ifndef __MESSAGE_H__
#define	__MESSAGE_H__

#define	MESSAGE_MAX		8


typedef struct {
	unsigned int		player_ID;
	unsigned int		command;
	unsigned int		arg[2];
} MESSAGE;


#endif
