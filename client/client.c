#include <string.h>

#include "muon.h"
#include "client.h"

void client_message_convert_send(MESSAGE_RAW *message) {
	message->player_id=darnitUtilHtonl(message->player_id);
	message->command=darnitUtilHtonl(message->command);
	message->arg_1=darnitUtilHtonl(message->arg_1);
	message->arg_2=darnitUtilHtonl(message->arg_2);
}

void client_message_convert_recv(MESSAGE_RAW *message) {
	message->player_id=darnitUtilNtohl(message->player_id);
	message->command=darnitUtilNtohl(message->command);
	message->arg_1=darnitUtilNtohl(message->arg_1);
	message->arg_2=darnitUtilNtohl(message->arg_2);
}

void client_check_incomming() {
	if(darnitSocketRecvTry(sock, msg_recv_offset, sizeof(MESSAGE_RAW)-(msg_recv_offset-(void*)&msg_recv))==sizeof(MESSAGE_RAW)-(msg_recv_offset-(void *)&msg_recv)) {
		msg_recv_offset=&msg_recv;
		client_message_convert_recv(&msg_recv);
		if(client_message_handler)
			client_message_handler(&msg_recv);
	}
}

void client_identify(MESSAGE_RAW *msg) {
	MESSAGE_RAW msg_send;
	msg_send.player_id=msg->player_id;
	msg_send.command=MSG_SEND_IDENTIFY;
	msg_send.arg_1=API_VERSION;
	msg_send.arg_2=strnlen(player_name, 32);
	client_message_convert_send(&msg_send);
	darnitSocketSend(sock, &msg_send, sizeof(MESSAGE_RAW));
	darnitSocketSend(sock, player_name, strnlen(player_name, 32));
	client_message_handler=NULL;
}

int client_init(char *host, int port) {
	if((sock=darnitSocketConnect(host, port))==NULL)
		return -1;
	msg_recv_offset=&msg_recv;
	client_message_handler=client_identify;
	return 0;
}
