#ifndef __PLAYER_H__
#define	__PLAYER_H__


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
} PLAYER;

PLAYER *playerInit(unsigned int players, int map_w, int map_h);
PLAYER *playerDestroy(PLAYER *player, int players);

int playerSlot(void *handle);

#endif
