#ifndef __PLAYER_H__
#define	__PLAYER_H__

#define PLAYER_ID_TIME				5

#define	PLAYER_NAME_LEN				32

#define	PLAYER_UNUSED				0
#define	PLAYER_WAITING_FOR_IDENTIFY		1
#define	PLAYER_IN_LOBBY				2

#define	PLAYER_PROCESS_NOTHING			0
#define	PLAYER_PROCESS_MSG			1
#define	PLAYER_PROCESS_DATA			2


typedef struct {
	unsigned int		power 	: 1;
	unsigned int		fog	: 1;
} PLAYER_MAP;


typedef struct {
	PLAYER_MAP		*map;
	int			status;
	SERVER_SOCKET		*socket;
	time_t			id_req_send;
	time_t			last_ping_sent;
	MESSAGE_BUFFER		*msg_buf;
	int			process_recv;
	MESSAGE			process_msg_recv;
	int			process_send;
	MESSAGE			process_msg_send;
	int			process_byte_send;
	char			name[PLAYER_NAME_LEN];
} PLAYER;

PLAYER *playerInit(unsigned int players, int map_w, int map_h);
PLAYER *playerDestroy(PLAYER *player, int players);
void playerDisconnect(unsigned int player);
void playerCheckIdentify();
void playerMessageBroadcast(unsigned int player, unsigned int command, unsigned int arg1, unsigned int arg2, void *data);
int playerSlot();

#endif
