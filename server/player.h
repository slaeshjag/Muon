/* Muon - a new kind of rts
 * Copyright 2012 Steven Arnow <s@rdw.se> and Axel Isaksson (mr3d/h4xxel)
 * 
 * This file is part of Muon.
 * 
 * Muon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Muon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Muon.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PLAYER_H__
#define	__PLAYER_H__

typedef enum PLAYER_DATA_TRANSFER {
	NONE,
	MAP,
	WORLD
} PLAYER_DATA_TRANSFER;

#define PLAYER_ID_TIME				5

#define	PLAYER_NAME_LEN				32

#define	PLAYER_UNUSED				0
#define	PLAYER_WAITING_FOR_IDENTIFY		1
#define	PLAYER_IN_LOBBY				2
#define	PLAYER_READY_TO_START			3
#define	PLAYER_IN_GAME_NOW			4
#define	PLAYER_SPECTATING			5
#define	PLAYER_BEING_DISCONNECTED		6

#define	PLAYER_BUILDSPOT_MULTIPLIER		5


typedef struct {
	unsigned int		power 	: 16;
	unsigned int		fog	: 16;
} PLAYER_MAP;


typedef struct {
	int			x;
	int			y;
	unsigned int		index;
} PLAYER_SPAWN;


typedef struct {
	int			building;
	int			time;
	int			progress;
	int			index;
	int			in_use;
	int			ready;
} PLAYER_BUILDQUEUE_E;


typedef struct {
	PLAYER_BUILDQUEUE_E	queue;
} PLAYER_BUILDQUEUE;


typedef struct {
	unsigned int		buildtime;
	unsigned int		no_build_time;
	unsigned int		buildings_raised;
	unsigned int		buildings_lost;
	unsigned int		buildings_destroyed;
	int			points;
	unsigned int		points_visible;
} PLAYER_STATS;


typedef struct {
	unsigned int		send_stat;
	unsigned int		send_pos;
	MESSAGE			send;	

	unsigned int		recv_stat;
	unsigned int		recv_pos;
	MESSAGE			recv;
	unsigned int		ready_to_send;
} PLAYER_NETWORK;


typedef struct {
	PLAYER_MAP		*map;
	unsigned int		map_progress;
	int			team;
	int			status;
	int			spawn_as;
	SERVER_SOCKET		*socket;
	time_t			id_req_send;
	time_t			last_ping_sent;
	time_t			last_ping_reply;
	MESSAGE_BUFFER		*msg_buf;
	char			name[PLAYER_NAME_LEN];
	PLAYER_NETWORK		network;
	PLAYER_SPAWN		spawn;
	PLAYER_BUILDQUEUE	queue;
	PLAYER_STATS		stats;
	int			buildspots;
	float			buildspeed;
	CONTROLPOINT_DATA	cp;

	PLAYER_DATA_TRANSFER	transfer;
	int			transfer_pos;
} PLAYER;

PLAYER *playerInit(unsigned int players, int map_w, int map_h);
PLAYER *playerDestroy(PLAYER *player, int players);
void playerDisconnect(unsigned int player);
void playerDisconnectKill(unsigned int player);
void playerCheckIdentify();
void playerBroadcastTeam(int team, unsigned int player, unsigned int command, unsigned int arg1, unsigned int arg2, void *data);
void playerMessageBroadcast(unsigned int player, unsigned int command, unsigned int arg1, unsigned int arg2, void *data);
int playerSlot();
int playerCalcLOS(unsigned int player, int x, int y, int mode);
int playerCalcSetPower(unsigned int player, int x, int y, int mode);

int playerBuildQueueInit();
int playerBuildQueueLoop(int msec);
int playerBuildQueueStart(int player, int building);
int playerBuildQueueStop(int player, int building);
int playerBuildQueueUnitReady(int player, int building);

void playerLoop();
void playerClear(int player);
void playerDefeatAnnounce(int player);
unsigned int playerCountPoints(int player);

int playerCanQueueAnotherBuilding(int player);

void playerKickAll();

#endif
