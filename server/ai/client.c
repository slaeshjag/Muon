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

#include <stdio.h>

#include "ai.h"
#include "client.h"

#ifdef _WIN32
#define _close -1+0*closesocket
#else
#define _close -1+0*close
#endif

#define _send(sock, buf, len) send((sock), (buf), (len), MSG_NOSIGNAL)

static SOCKET _connectsocket(const char *host, int port) {
	SOCKET sock;
	struct hostent *hp;
	struct sockaddr_in sin={0};
	
	if((sock=socket(AF_INET, SOCK_STREAM, 0))<0)
		return -1;
	
	if(!(hp=gethostbyname(host)))
		return _close(sock);
	
	sin.sin_family=AF_INET;
	sin.sin_port=htons(port);
	#ifdef _WIN32
		sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	#else
		sin.sin_addr.s_addr = *(unsigned long *) hp->h_addr_list[0];
	#endif
	
	if(connect(sock, (void *) &sin, sizeof(struct sockaddr_in))==-1)
		return _close(sock);
	
	#ifdef _WIN32
	u_long mode=1;
	ioctlsocket(sock, FIONBIO, &mode);
	#else
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0)|O_NONBLOCK);
	#endif
	
	return sock;
}

static int _recv(SOCKET sock, char *buf, int len) {
	int ret;

	if((ret=recv(sock, buf, len, MSG_PEEK|MSG_NOSIGNAL))==len) {
		recv(sock, buf, len, MSG_NOSIGNAL);
		return len;
	}
	if (ret>-1)
		return 0;
	#ifndef _WIN32
	if(errno==EAGAIN||errno==EWOULDBLOCK)
		return 0;
	#else
	int error_n=WSAGetLastError();
	if(error_n==WSAEWOULDBLOCK)
		return 0;
	#endif
	return -1;
}


SOCKET client_connect() {
	SOCKET sock;
	/*change later to use the actual port the server uses*/
	sock=_connectsocket("127.0.0.1", SERVER_PORT_DEFAULT);
	return sock;
}

void client_disconnect(SOCKET sock) {
	_close(sock);
}

void client_message_convert_send(MESSAGE_RAW *message) {
	message->player_id=htonl(message->player_id);
	message->command=htonl(message->command);
	message->arg_1=htonl(message->arg_1);
	message->arg_2=htonl(message->arg_2);
}

void client_message_convert_recv(MESSAGE_RAW *message) {
	message->player_id=ntohl(message->player_id);
	message->command=ntohl(message->command);
	message->arg_1=ntohl(message->arg_1);
	message->arg_2=ntohl(message->arg_2);
}

void client_message_send(SOCKET sock, int player_id, int command, int arg_1, int arg_2, unsigned char *payload) {
	MESSAGE_RAW msg_send;
	msg_send.player_id=player_id;
	msg_send.command=command;
	msg_send.arg_1=arg_1;
	msg_send.arg_2=arg_2;
	client_message_convert_send(&msg_send);
	_send(sock, &msg_send, sizeof(MESSAGE_RAW));
	if(payload)
		_send(sock, payload, arg_2);
}

void client_check_incoming() {
	
}
