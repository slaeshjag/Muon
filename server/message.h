#ifndef __MESSAGE_H__
#define	__MESSAGE_H__

#define	MESSAGE_MAX		8


typedef struct {
	unsigned int		player_ID;
	unsigned int		command;
	unsigned int		arg_1;
	unsigned int		arg_2;
} MESSAGE;


#endif
