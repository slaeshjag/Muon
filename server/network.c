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

#include "server.h"


int networkInit() {
	#ifdef _WIN32
	
	WSADATA wsaData;
	WORD version;
	struct hostent *hp;
	u_long iMode=1;
	
	version = MAKEWORD(2, 0);
	if (WSAStartup(version, &wsaData) != 0) {
		return -1;
	} else if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0) {
		WSACleanup();
		return -1;
	}
	#endif
	
	return 0;
}


SERVER_SOCKET *networkSocketDisconnect(SERVER_SOCKET *sock) {
	if (!sock)
		return NULL;
	#ifndef _WIN32
	close(sock->socket);
	#else
	closesocket(sock->socket);
	#endif
	

	free(sock);
	return NULL;
}	


SERVER_SOCKET *networkListen(int port) {
	SERVER_SOCKET *sock;
	int flags;

	if ((sock = malloc(sizeof(SERVER_SOCKET))) == NULL)
		return NULL;
	

	struct sockaddr_in address;
	#ifdef _WIN32
	u_long iMode = 1;
	#endif
	
	if ((sock->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Unable to create a socket\n");
		free(sock);
		return NULL;
	}
	
	memset(&address, 0, sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = 0x0;		/* Listen on ALL the interfaces! */

	if (bind(sock->socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
		fprintf(stderr, "Unable to listen on *:%i\n", port);
		free(sock);
		return NULL;
	}

	if (listen(sock->socket, 2) < 0) {
		fprintf(stderr, "Unable to listen on *:%i\n", port);
		#ifdef _WIN32
		closesocket(sock->socket);
		#else
		close(sock->socket);
		#endif
		free(sock);
		return NULL;
	}
	#ifndef _WIN32
	if ((flags = fcntl(sock->socket, F_GETFL, 0)) < 0)
		flags = 0;
	if (fcntl(sock->socket, F_SETFL, flags | O_NONBLOCK) < 0)
		return networkSocketDisconnect(sock);
	#else
	ioctlsocket(sock->socket, FIONBIO, &iMode);
	#endif


	return sock;
}


SERVER_SOCKET *networkAccept(SERVER_SOCKET *sock) {
	SERVER_SOCKET *sock_a;
	
	if (!sock)
		return NULL;

	#ifndef _WIN32
	int socket;
	#else
	SOCKET socket;
	u_long iMode = 1;
	#endif
	int address_len, flags;
	struct sockaddr_in address;
	address_len = sizeof(struct sockaddr_in);

	if ((socket = accept(sock->socket, (struct sockaddr *) &address, (unsigned int *) &address_len)) == INVALID_SOCKET)
		return NULL;

	if ((sock_a = malloc(sizeof(SERVER_SOCKET))) == NULL)
		goto error;

	sock_a->socket = socket;
	#ifndef _WIN32
	
	if ((flags = fcntl(sock_a->socket, F_GETFL, 0)) < 0)
		flags = 0;
	if (fcntl(sock_a->socket, F_SETFL, flags | O_NONBLOCK) < 0)
		return networkSocketDisconnect(sock_a);
	#else
	ioctlsocket(sock->socket, FIONBIO, &iMode);
	#endif

	return sock_a;

	error:

	#ifndef _WIN32
	close(socket);
	#else
	closesocket(socket);
	#endif
	
	return NULL;
}


int networkReceive(SERVER_SOCKET *sock, char *buff, int buff_len) {
	int t;

	if (!sock)
		return -1;

	if ((t = recv(sock->socket, buff, buff_len, 0)) < 0) {
		#ifndef _WIN32
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			return -1;
		#else
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			return -1;
		#endif
		else
			return 0;
	}

	return t;
}


int networkReceiveTry(SERVER_SOCKET *sock, char *buff, int buff_len) {
	int t;

	if (!sock)
		return -1;
	
	if ((t = recv(sock->socket, buff, buff_len, MSG_PEEK)) == buff_len) {
		recv(sock->socket, buff, buff_len, 0);
		return t;
	}

	if (t > -1)		/* Should be impossible */
		return 0;
	#ifndef _WIN32
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		return 0;
	#else
	int error_n = WSAGetLastError();
	if (error_n == WSAEWOULDBLOCK || error_n == WSAEINPROGRESS || error_n == WSAEALREADY)
		return 0;
	#endif
	return -1;
}


int networkSend(SERVER_SOCKET *sock, char *buff, int buff_len) {
	int t;

	if (!sock)
		return -1;

	t = send(sock->socket, buff, buff_len, MSG_NOSIGNAL);

	if (t >= 0)
		return t;

	#ifndef _WIN32
	if (errno != EAGAIN && errno != EWOULDBLOCK) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return -1;
	}
	#else
	int error_n = WSAGetLastError();
	if (error_n != WSAEWOULDBLOCK && error_n != WSAEINPROGRESS)
		return -1;
	#endif
	return 0;
}


SERVER_SOCKET_SELECT *networkSelectInit() {
	SERVER_SOCKET_SELECT *sel;

	if ((sel = malloc(sizeof(SERVER_SOCKET_SELECT))) == NULL)
		return NULL;
	
	sel->read = sel->write = NULL;
	FD_ZERO(&sel->set_r);
	FD_ZERO(&sel->set_w);
	sel->max = 0;
	sel->timeout.tv_sec = sel->timeout.tv_usec = 0;

	return sel;
}


int networkSelectAdd(SERVER_SOCKET_SELECT_N **sel, SERVER_SOCKET *sock) {
	SERVER_SOCKET_SELECT_N *sel_n;

	if ((sel_n = malloc(sizeof(SERVER_SOCKET_SELECT_N))) == NULL)
		return -1;
	sel_n->sock = sock;
	sel_n->next = *sel;
	*sel = sel_n;

	return 0;
}	


void networkSelectAddWrite(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock) {
	if (!sock)
		return;
	if (networkSelectAdd(&sel->write, sock) < 0)
		return;
	
	if (sock->socket > sel->max)
		sel->max = sock->socket;

	FD_SET(sock->socket, &sel->set_w);
	
	return;
}


void networkSelectAddRead(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock) {
	if (!sock)
		return;
	if (networkSelectAdd(&sel->read, sock) < 0)
		return;
	
	if (sock->socket > sel->max)
		sel->max = sock->socket;
	
	FD_SET(sock->socket, &sel->set_r);

	return;
}


int networkSelectMax(SERVER_SOCKET_SELECT_N *sel, fd_set *set) {
	int max = 0;

	FD_ZERO(set);

	while (sel != NULL) {
		if (sel->sock->socket > max)
			max = sel->sock->socket;
		FD_SET(sel->sock->socket, set);
		sel = sel->next;
	}

	return max;
}


void networkSelectRecalc(SERVER_SOCKET_SELECT *sel) {
	int max = 0;

	sel->max = networkSelectMax(sel->read, &sel->set_r);
	if ((max = networkSelectMax(sel->write, &sel->set_w)) > sel->max)
		sel->max = max;

	return;
}

void networkSelectRemove(SERVER_SOCKET_SELECT *sel_m, SERVER_SOCKET_SELECT_N **sel, SERVER_SOCKET *sock) {
	SERVER_SOCKET_SELECT_N **sel_t, *sel_n;

	sel_t = sel;
	while (*sel_t != NULL) {
		if ((*sel_t)->sock == sock) {
			sel_n = *sel_t;	
			*sel_t = (sel_n)->next;
			free(sel_n);
		}
		sel_t = &(*sel_t)->next;
	}

	networkSelectRecalc(sel_m);

	return;
}


void networkSelectRemoveRead(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock) {
	if (!sel || !sock)
		return;
	networkSelectRemove(sel, &sel->read, sock);
	
	return;
}


void networkSelectRemoveWrite(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock) {
	if (!sel || !sock)
		return;
	networkSelectRemove(sel, &sel->write, sock);
	
	return;
}


int networkSelect(SERVER_SOCKET_SELECT *sel) {
	int num;

	if (!sel)
		return -1;
	sel->tmp_w = sel->set_w;
	sel->tmp_r = sel->set_r;
	sel->timeout_t = sel->timeout_t;

	num = select(sel->max, &sel->tmp_w, &sel->tmp_r, NULL, &sel->timeout_t);
	return num;
}


int networkSelectTestWrite(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock) {
	if (!sock || !sel)
		return -1;
	return FD_ISSET(sock->socket, &sel->tmp_w);
}


int networkSelectTestRead(SERVER_SOCKET_SELECT *sel, SERVER_SOCKET *sock) {
	if (!sock || !sel)
		return -1;
	return FD_ISSET(sock->socket, &sel->tmp_r);
}

