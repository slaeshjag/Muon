#ifndef __SERVER_H__
#define	__SERVER_H__

#define	SERVER_PORT		56789

#include "unit.h"
#include "message.h"
#include "player.h"
#include "network.h"

#include <stdio.h>
#include <stdlib.h>


struct SERVER_UNIT {
	unsigned int		owner;
	unsigned int		type;
	unsigned int		hp;
	unsigned int		shield;
	unsigned int		powered;
	unsigned int		status;
	unsigned int		target;
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
	PLAYER			*player;
	unsigned int		players;
	SERVER_SOCKET		*accept;
} SERVER;

int serverPowerGet(SERVER *server, int owner, int x, int y);
SERVER *serverInit(const int map_w, const int map_h, unsigned int players, int port);
SERVER *serverDestroy(SERVER *server);
int serverLoop(SERVER *server, unsigned int d_ms);

#endif
