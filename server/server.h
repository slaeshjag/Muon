#ifndef __SERVER_H__
#define	__SERVER_H__

#include "unit.h"
#include "message.h"


typedef struct SERVER_UNIT {
	unsigned int		owner;
	unsigned int		type;
	unsigned int		hp;
	unsigned int		shield;
	unsigned int		powered;
	unsigned int		status;
	SERVER_UNIT		*next;
};


typedef struct {
	SERVER_UNIT		*unit;
	unsigned int		w;
	unsigned int		h;
	SERVER_UNIT		**map;
} SERVER;


#endif
