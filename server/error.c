#define WITH_SERVER_ERROR
#include "server.h"


int errorInit() {
	server_error.read_pos = 0;
	server_error.write_pos = 0;

	return 0;
}


void errorPush(int error) {
	server_error.err[server_error.write_pos] = error;

	server_error.write_pos++;
	if (server_error.write_pos == SERVER_ERROR_MAX_POS)
		server_error.write_pos = 0;
	return;
}


const char *errorPop() {
	int err;

	if (server_error.read_pos == server_error.write_pos)
		err = 0;
	else {
		err = server_error.err[server_error.read_pos];
		server_error.read_pos++;
		if (server_error.read_pos == SERVER_ERROR_MAX_POS)
			server_error.read_pos = 0;
	}

	return server_errors[err];
}
