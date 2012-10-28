#include "server_tester.h"

void messageConvert(MESSAGE *msg) {
	msg->player_ID = htonl(msg->player_ID);
	msg->command = htonl(msg->command);
	msg->arg[0] = htonl(msg->arg[0]);
	msg->arg[1] = htonl(msg->arg[1]);
	
	return;
}


int main(int argc, char **argv) {
	MESSAGE message;
	struct sockaddr_in address;
	struct hostent *hp;
	char buff[256];
	int sock, port, i;
	
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

	sleep(8);
	
	return 0;
}
