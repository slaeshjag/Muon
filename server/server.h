#ifndef __SERVER_H__
#define	__SERVER_H__

#define	SERVER_PORT		56789
#define	SERVER_VERSION		0x10000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "network.h"
#include "unit.h"
#include "message.h"
#include "message_handlers.h"
#include "player.h"
#include "game.h"
#include "lobby.h"
#ifndef __TTMZ_H__
#include "map.h"
#endif


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
	MESSAGE_HANDLER		message_handler;
	unsigned int		w;
	unsigned int		h;
	SERVER_UNIT		**map;
	unsigned int		*power_grid;
	UNIT_DEF		*unit_def;
	MESSAGE_BUFFER		*message_buffer;
	PLAYER			*player;
	unsigned int		players;
	GAME			game;
	SERVER_SOCKET		*accept;
	void			*map_data;
} SERVER;

SERVER *server;


int serverPowerGet(int owner, int x, int y);
SERVER *serverInit(const char *fname, unsigned int players, int port);
SERVER *serverDestroy();
int serverLoop(unsigned int d_ms);

#endif
