#ifndef __SERVER_H__
#define	__SERVER_H__

#include "unit.h"
#include "message.h"

#include <stdio.h>
#include <stdlib.h>


struct SERVER_UNIT {
	unsigned int		owner;
	unsigned int		type;
	unsigned int		hp;
	unsigned int		shield;
	unsigned int		powered;
	unsigned int		status;
	struct SERVER_UNIT	*next;
};

typedef struct SERVER_UNIT SERVER_UNIT;


typedef struct {
	SERVER_UNIT		*unit;
	unsigned int		w;
	unsigned int		h;
	SERVER_UNIT		**map;
	unsigned int		*power_grid;
	UNIT_DEF		*unit_def;
	MESSAGE_BUFFER		*message_buffer;
	unsigned int		players;
} SERVER;


#endif
