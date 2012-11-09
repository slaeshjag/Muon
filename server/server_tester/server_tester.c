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

#include "server_tester.h"

void messageConvert(MESSAGE *msg) {
	msg->player_ID = htonl(msg->player_ID);
	msg->command = htonl(msg->command);
	msg->arg[0] = htonl(msg->arg[0]);
	msg->arg[1] = htonl(msg->arg[1]);
	
	return;
}


void messageSend(int sock, unsigned int command, unsigned int arg1, unsigned int arg2) {
	MESSAGE msg;

	msg.player_ID = 0;
	msg.command = htonl(command);
	msg.arg[0] = htonl(arg1);
	msg.arg[1] = htonl(arg2);

	send(sock, &msg, 16, 0);

	return;
}


int main(int argc, char **argv) {
	MESSAGE message;
	struct sockaddr_in address;
	struct hostent *hp;
	char buff[256];
	int sock, port, i, a, j;
	
	if (argc <3) {
		printf("Usage: %s <host> <port>\n", argv[0]);
		return -1;
	}
	
	port = atoi(argv[2]);
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Failed to create a socket.\n");
		return -1;
	}
	
	if ((hp = gethostbyname(argv[1])) == NULL) {
		fprintf(stderr, "Failed to resolve host '%s'\n", argv[1]);
		return -1;
	}
	
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = *( u_long * ) hp->h_addr;
	
	if (connect(sock, (struct sockaddr *) &address, sizeof(struct sockaddr)) == -1) {
			fprintf(stderr, "Failed to connect to host '%s'\n",
				argv[1]);
			return -1;
	}
	
	message.player_ID = 0;
	message.command = 2;
	message.arg[0] = 0x10000;
	message.arg[1] = strlen("Testspelare");
	messageConvert(&message);
	
	send(sock, &message, 16, 0);
	send(sock, "Testspelare", strlen("Testspelare"), 0);

	for (j = 0;; j++) {
		if (recv(sock, &message, 16, 0) <= 0)
			break;
		messageConvert(&message);
		fprintf(stderr, "\n\nMessage %i from player %i: %i %i;; ", message.command, message.player_ID, message.arg[0], message.arg[1]);

		if (message.command & 0x100)
			for (i = 0; i < message.arg[1]; ) {
					a = (message.arg[1] - i > 256) ? 256 : message.arg[1] - i;
				
				if ((port = recv(sock, buff, a, 0)) <= 0)
					break;
				i += port;
				fwrite(buff, port, 1, stderr);

			}
		if (j == 10) {
			fprintf(stderr, "\nSending ready message\n");
			messageSend(sock, 4, 1, 100);
		}

		if (port <= 0)
			break;
	}

	close(sock);

	return 0;
}
