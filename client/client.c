#include <stdio.h>
#include <string.h>

#include "muon.h"
#include "client.h"
#include "view.h"

#define PONG case MSG_RECV_PING: client_message_send(player_id, MSG_SEND_PONG, 0, 0, NULL); break

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

void client_message_send(int player_id, int command, int arg_1, int arg_2, char *payload) {
	MESSAGE_RAW msg_send;
	msg_send.player_id=player_id;
	msg_send.command=command;
	msg_send.arg_1=arg_1;
	msg_send.arg_2=arg_2;
	client_message_convert_send(&msg_send);
	darnitSocketSend(sock, &msg_send, sizeof(MESSAGE_RAW));
	if(payload)
		darnitSocketSend(sock, payload, arg_2);
}

int client_check_incomming() {
	int s, i;
	for(i=0; i<1000; i++) {
		if(msg_recv.command&MSG_PAYLOAD_BIT) {
			//download message payload
			if((s=darnitSocketRecvTry(sock, msg_recv_payload, msg_recv.arg_2))) {
				if(s==0)
					return 0;
				if(s==-1) {
					sock=darnitSocketClose(sock);
					return -1;
				}
				printf("payload size %i\n", msg_recv.arg_2);
				if(client_message_handler)
					client_message_handler(&msg_recv, msg_recv_payload);
				msg_recv.command=0;
			}
		} else {
			//download message
			if((s=darnitSocketRecvTry(sock, msg_recv_offset, sizeof(MESSAGE_RAW)))) {
				if(s==0)
					return 0;
				if(s==-1) {
					sock=darnitSocketClose(sock);
					return -1;
				}
				client_message_convert_recv(&msg_recv);
				printf("message: 0x%x\n", msg_recv.command);
				if(client_message_handler&&!(msg_recv.command&MSG_PAYLOAD_BIT))
					client_message_handler(&msg_recv, NULL);
			}
		}
	}
	return 0;
}

void client_game_handler(MESSAGE_RAW *msg, unsigned char *payload) {
	char *chatmsg;
	switch(msg->command) {
		PONG;
		case MSG_RECV_CHAT:
			chatmsg=(char *)malloc(msg->arg_2+1);
			memcpy(chatmsg, payload, msg->arg_2);
			chatmsg[msg->arg_2]=0;
			printf("<%s> %s\n", &player_names[msg->player_id*32], chatmsg);
			free(chatmsg);
			break;
	}
}

void client_countdown_handler(MESSAGE_RAW *msg, unsigned char *payload) {
	switch(msg->command) {
		PONG;
		case MSG_RECV_GAME_START:
			printf("Game starts in %i\n", msg->arg_1);
			UI_PROPERTY_VALUE v={.p=countdown_text};
			countdown_text[0]=(char)(msg->arg_1)+0x30;
			countdown_text[1]=0;
			panelist_countdown.pane->root_widget->set_prop(panelist_countdown.pane->root_widget, UI_LABEL_PROP_TEXT, v);
			if(!msg->arg_1) {
				state=GAME_STATE_GAME;
				client_message_handler=client_game_handler;
			}
			break;
	}
}

void client_download_map(MESSAGE_RAW *msg, unsigned char *payload) {
	static int filesize_bytes=0, downloaded_bytes=0;
	static char *filename=NULL;
	static DARNIT_FILE *f;
	switch(msg->command) {
		PONG;
		case MSG_RECV_JOIN:
			if(!payload)
				break;
			memcpy(&player_names[msg->player_id*32], payload, msg->arg_2);
			printf("Player %s joined the game\n", &player_names[msg->player_id*32]);
			break;
		case MSG_RECV_MAP_BEGIN:
			if(!payload)
				break;
			if(!filename)
				free(filename);
			filesize_bytes=msg->arg_1;
			filename=malloc(msg->arg_2+1);
			memcpy(filename, payload, msg->arg_2);
			filename[msg->arg_2]=0;
			f=darnitFileOpen(filename, "wb");
			printf("Started map download of file %s\n", filename);
			break;
		case MSG_RECV_MAP_CHUNK:
			if(!payload)
				break;
			downloaded_bytes+=msg->arg_2;
			darnitFileWrite(payload, msg->arg_2, f);
			client_message_send(player_id, MSG_SEND_READY, 0, 100*downloaded_bytes/filesize_bytes, NULL);
			printf("Map progress %i%%\n", 100*downloaded_bytes/filesize_bytes);
			break;
		case MSG_RECV_MAP_END:
			darnitFileClose(f);
			printf("Map %s successfully downloaded!\n", filename);
			//client_message_send(player_id, MSG_SEND_MAP_PROGRESS, 100, 0, NULL);
			client_message_send(player_id, MSG_SEND_READY, 1, 100, NULL);
			darnitFSMount(filename);
			map=darnitMapLoad("maps/map.ldmz");
			map_w=map->layer->tilemap->w*map->layer->tile_w;
			map_h=map->layer->tilemap->h*map->layer->tile_h;
			state=GAME_STATE_COUNTDOWN;
			client_message_handler=client_countdown_handler;
			break;
	}
}

void client_identify(MESSAGE_RAW *msg, unsigned char *payload) {
	player_id=msg->player_id;
	player_names=(char *)calloc(msg->arg_2, 32);
	client_message_send(player_id, MSG_SEND_IDENTIFY, API_VERSION, strnlen(player_name, 32), player_name);
	client_message_handler=client_download_map;
}

int client_init(char *host, int port) {
	if((sock=darnitSocketConnect(host, port))==NULL)
		return -1;
	msg_recv.command=0;
	msg_recv_payload_offset=msg_recv_payload;
	msg_recv_offset=&msg_recv;
	client_message_handler=client_identify;
	return 0;
}
