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
#include <time.h>
#include <limits.h>

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

static int _recv(SOCKET sock, void *buf, int len) {
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

void client_disconnect(AI *ai) {
	_close(ai->sock);
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

void client_message_send(AI *ai, int player_id, int command, int arg_1, int arg_2, unsigned char *payload) {
	MESSAGE_RAW msg_send;
	msg_send.player_id=player_id;
	msg_send.command=command;
	msg_send.arg_1=arg_1;
	msg_send.arg_2=arg_2;
	client_message_convert_send(&msg_send);
	_send(ai->sock, &msg_send, sizeof(MESSAGE_RAW));
	if(payload)
		_send(ai->sock, payload, arg_2);
}

void client_check_incoming(AI *ai) {
	int s;
	unsigned int chunk_got=0, chunk_size, chunk_time;
	static unsigned int chunk_size_time=UINT_MAX;
	if((s=_recv(ai->sock, &chunk_size, 4))<4) {
		if(s==-1)
			client_disconnect(ai);
		if(time(NULL)-chunk_size_time>CLIENT_TIMEOUT) {
			client_disconnect(ai);
			return;
		} else
			return;
	} else
		chunk_size_time=time(NULL);
	chunk_size=htonl(chunk_size)-4;
	chunk_time=time(NULL);
	
	while(chunk_got<chunk_size) {
		if(time(NULL)-chunk_time>CLIENT_TIMEOUT) {
			client_disconnect(ai);
			return;
		}
		if(msg_recv.command&MSG_PAYLOAD_BIT) {
			//download message payload
			//printf("payload size %i\n", msg_recv.arg_2);
			s=_recv(ai->sock, msg_recv_payload, msg_recv.arg_2);
			if(s==0)
				continue;
			if(s==-1) {
				client_disconnect(ai);
				return;
			}
			if(ai->message)
				if(ai->message(ai, &msg_recv, msg_recv_payload)) {
					chunk_size_time=UINT_MAX;
					return;
				}
			msg_recv.command=0;
		} else {
			//download message
			s=_recv(ai->sock, msg_recv_offset, sizeof(MESSAGE_RAW));
			if(s==0) {
				msg_recv.command = 0;
				continue;
			}
			if(s==-1) {
				client_disconnect(ai);
				return;
			}
			client_message_convert_recv(&msg_recv);
			//printf("message: 0x%x (%i, %i) to player %i\n", msg_recv.command, msg_recv.arg_1, msg_recv.arg_2, msg_recv.player_id);
			if(ai->message&&!(msg_recv.command&MSG_PAYLOAD_BIT))
				if(ai->message(ai, &msg_recv, NULL)) {
					chunk_size_time=UINT_MAX;
					return;
				}
		}
		chunk_got+=s;
	}
	client_message_send(ai, ai->player_id, MSG_SEND_CHUNK_OK, 0, 0, NULL);
}
