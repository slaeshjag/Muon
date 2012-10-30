#ifndef __SERVER_TESTER_H__
#define	__SERVER_TESTER_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>


typedef struct {
	unsigned int			player_ID;
	unsigned int			command;
	unsigned int			arg[2];
} MESSAGE;


#endif
