#ifndef __NETWORK_H__
#define	__NETWORK_H__

#ifndef _WIN32
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <fcntl.h>
#define	INVALID_SOCKET			-1
#else
#include <windows.h>
#include <winsock2.h>

#define	EWOULDBLOCK			WSAEWOULDBLOCK
#define	MSG_NOSIGNAL			0
#endif
#include <errno.h>


typedef struct {
	#ifndef _WIN32
	int				socket;
	#else
	SOCKET				socket;
	#endif
} SERVER_SOCKET;


typedef struct SERVER_SOCKET_SELECT_N {
	SERVER_SOCKET			*sock;
	struct SERVER_SOCKET_SELECT_N	*next;
} SERVER_SOCKET_SELECT_N;


typedef struct {
	SERVER_SOCKET_SELECT_N		*read;
	SERVER_SOCKET_SELECT_N		*write;
	fd_set				set_w;
	fd_set				set_r;

	fd_set				tmp_w;
	fd_set				tmp_r;
	struct timeval			timeout;
	struct timeval			timeout_t;
	int				max;
} SERVER_SOCKET_SELECT;


int networkInit();
SERVER_SOCKET *networkSocketDisconnect(SERVER_SOCKET *sock);
SERVER_SOCKET *networkListen(int port);
SERVER_SOCKET *networkAccept(SERVER_SOCKET *sock);
int networkReceive(SERVER_SOCKET *sock, char *buff, int buff_len);
int networkReceiveTry(SERVER_SOCKET *sock, char *buff, int buff_len);
int networkSend(SERVER_SOCKET *sock, char *buff, int buff_len);
SERVER_SOCKET_SELECT *networkSelectInit();
int networkSelectAdd(SERVER_SOCKET_SELECT_N **sel, SERVER_SOCKET *sock);
void networkSelectAddWrite(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock);
void networkSelectAddRead(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock);
int networkSelectMax(SERVER_SOCKET_SELECT_N *sel, fd_set *set);
void networkSelectRecalc(SERVER_SOCKET_SELECT *sel);
void networkSelectRemove(SERVER_SOCKET_SELECT *sel_m, SERVER_SOCKET_SELECT_N **sel, SERVER_SOCKET *sock);
void networkSelectRemoveRead(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock);
void networkSelectRemoveWrite(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock);
int networkSelect(SERVER_SOCKET_SELECT *sel);
int networkSelectTestWrite(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock);
int networkSelectTestRead(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock);


#endif
