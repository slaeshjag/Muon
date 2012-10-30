#ifndef __GAME_H__
#define	__GAME_H__


#ifdef __DEBUG__
#define	SERVER_GAME_START_DELAY	1000
#else
#define	SERVER_GAME_START_DELAY	5000
#endif

#define	SERVER_GAME_COUNTDOWN	(SERVER_GAME_START_DELAY/1000)


typedef struct {
	int			started;
	unsigned int		time_elapsed;
	unsigned int		countdown;
} GAME;

int gameInit();
void gameLoop(int msec);
int gameKillPlayerUnits(unsigned int player);
int gameAttemptStart();
void gameeStart();

#endif
