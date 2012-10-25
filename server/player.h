#ifndef __PLAYER_H__
#define	__PLAYER_H__


typedef struct {
	unsigned int		power 	: 1;
	unsigned int		fog	: 1;
} PLAYER_MAP;


typedef struct {
	PLAYER_MAP		*map;
} PLAYER;

PLAYER *playerInit(unsigned int players, int map_w, int map_h);
PLAYER *playerDestroy(PLAYER *player, int players);


#endif
