#include "server.h"


void messageHandlerDummy(unsigned int player, MESSAGE *message) {
	fprintf(stderr, "MSG_RECV: %i  - Not implemented yet!\n", message->command);
	
	return;
}


int messageHandlerInit() {
	server->message_handler.handle[0] = messageHandlerDummy;
	server->message_handler.handle[1] = messageHandlerDummy;
	server->message_handler.handle[2] = messageHandlerDummy;
	server->message_handler.handle[3] = messageHandlerDummy;
	server->message_handler.handle[4] = messageHandlerDummy;
	server->message_handler.handle[5] = messageHandlerDummy;
	server->message_handler.handle[6] = messageHandlerDummy;
	server->message_handler.handle[7] = messageHandlerDummy;

	return 0;
}
