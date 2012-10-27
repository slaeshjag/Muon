#include "server.h"

int main(int argc, char **argv) {
	int port;

	if (argc <3) {
		fprintf(stdout, "Usage: %s <map> <players> [port]\n", argv[0]);
		return -1;
	}

	if (argc >= 4)
		port = atoi(argv[3]);
	else
		port = SERVER_PORT;
	
	if (serverInit(argv[1], atoi(argv[2]), port) == NULL)
		return -1;
	for (;;) {
		sleep(1);
		serverLoop(1000);
	}

	
	return 0;
}
