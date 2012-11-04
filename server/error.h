#ifndef __ERROR_H__
#define	__ERROR_H__

#define	SERVER_ERROR_CANT_OPEN_MAP		1
#define	SERVER_ERROR_NO_MEMORY			2
#define	SERVER_ERROR_SPAWNS_OVERLAP		3
#define	SERVER_ERROR_INVALID_MAP		4
#define	SERVER_ERROR_MAP_NO_MAX_PLAYERS		5
#define	SERVER_ERROR_UNABLE_TO_LISTEN		6
#define	SERVER_ERROR_TOO_MANY_PLAYERS		7
#define	SERVER_ERROR_GAMESPEED_TOO_SMALL	8
#define	SERVER_ERROR_SPAWN_MISSING		9

#ifdef WITH_SERVER_ERROR

#define	SERVER_ERROR_MAX_POS	8

const char *server_errors[] = { NULL, 
				"Unable to open the map file for caching.",
				"Unable to allocate more memory. Your system might've runned out of RAM.",
				"The map contains overlapping spawn points. This is not acceptable.",
				"The map file is invalid.",
				"The map is missing the property 'max_players'",
				"The server was unable to bind the requested port. It's either in use or blocked by a firewall.",
				"The map is not defined for this many players",
				"Gamespeed must be bigger than 0",
				"The map is missing a generator for one or more players"};

typedef struct {
	int			read_pos;
	int			write_pos;
	int			err[SERVER_ERROR_MAX_POS];
} SERVER_ERROR;


SERVER_ERROR server_error;

#endif

int errorInit();
void errorPush(int error);
const char *errorPop();


#endif
