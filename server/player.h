#ifndef __PLAYER_H__
#define	__PLAYER_H__

#define PLAYER_ID_TIME				5

#define	PLAYER_UNUSED				0
#define	PLAYER_WAITING_FOR_IDENTIFY		1

typedef struct {
	unsigned int		power 	: 1;
	unsigned int		fog	: 1;
} PLAYER_MAP;


typedef struct {
	PLAYER_MAP		*map;
	int			status;
	SERVER_SOCKET		*socket;
	time_t			id_req_send;
} PLAYER;

PLAYER *playerInit(unsigned int players, int map_w, int map_h);
PLAYER *playerDestroy(PLAYER *player, int players);

int playerSlot();

#endif
