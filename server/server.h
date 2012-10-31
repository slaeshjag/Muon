#ifndef __SERVER_H__
#define	__SERVER_H__

#define	SERVER_PORT		56789
#define	SERVER_VERSION		0x10000

#define	SERVER_PING_INTERVAL	2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "network.h"
#include "unit.h"
#include "message.h"
#include "message_handlers.h"
#include "player.h"
#include "game.h"
#include "lobby.h"
#include "error.h"
#ifndef __TTMZ_H__
#include "map.h"
#endif


typedef struct SERVER_UNIT {
	unsigned int		owner;
	unsigned int		type;
	unsigned int		hp;
	unsigned int		shield;
	unsigned int		status;
	unsigned int		target;
	UNIT_PYLON		pylon;
	struct SERVER_UNIT	*next;
} SERVER_UNIT;


typedef struct {
	const char		*path;
	void			*data;
	unsigned int		data_len;
	unsigned int		*tile_data;
} SERVER_MAP;


typedef struct {
	SERVER_UNIT		*unit;
	MESSAGE_HANDLER		message_handler;
	unsigned int		w;
	unsigned int		h;
	SERVER_UNIT		**map;
	unsigned int		*power_grid;
	UNIT_DEF		*unit_def;
	PLAYER			*player;
	unsigned int		players;
	GAME			game;
	SERVER_SOCKET		*accept;
	void			*map_data;
	SERVER_MAP		map_c;
	int			build_spots;
	UNIT_PYLON_LIST		*pylons;
} SERVER;

SERVER *server;


int serverPowerGet(int owner, int x, int y);
SERVER *serverStart(const char *fname, unsigned int players, int port);
SERVER *serverStop();
int serverLoop(unsigned int d_ms);

#endif
