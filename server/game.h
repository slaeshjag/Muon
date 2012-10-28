#ifndef __GAME_H__
#define	__GAME_H__


typedef struct {
	int			started;
} GAME;

int gameInit();
int gameKillPlayerUnits(unsigned int player);
int gameAttemptStart();


#endif
