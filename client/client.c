#include <stdio.h>
#include <string.h>

#include "muon.h"
#include "client.h"
#include "view.h"
#include "map.h"
#include "game.h"
#include "chat.h"
#include "lobby.h"

#define PONG case MSG_RECV_PING: client_message_send(player_id, MSG_SEND_PONG, 0, 0, NULL); break

unsigned int recalc_map=0;

void client_connect_callback(int ret, void *data, void *socket) {
	if(ret) {
		free(player_names);
		game_state(GAME_STATE_CONNECT_SERVER);
		darnitSocketClose(socket);
	} else
		game_state(GAME_STATE_LOBBY);
}

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

void client_check_incomming() {
	int s, i;
	for(i=0; i<1000; i++) {
		if(msg_recv.command&MSG_PAYLOAD_BIT) {
			//download message payload
			s=darnitSocketRecvTry(sock, msg_recv_payload, msg_recv.arg_2);
				if(s==0)
					break;
				if(s==-1) {
					client_connect_callback(-1, NULL, sock);
					return;
				}
				//printf("payload size %i\n", msg_recv.arg_2);
				if(client_message_handler)
					client_message_handler(&msg_recv, msg_recv_payload);
				msg_recv.command=0;
		} else {
			//download message
			s=darnitSocketRecvTry(sock, msg_recv_offset, sizeof(MESSAGE_RAW));
			if(s==0)
				break;
			if(s==-1) {
				client_connect_callback(-1, NULL, sock);
				return;
			}
			client_message_convert_recv(&msg_recv);
			//printf("message: 0x%x (%i, %i)\n", msg_recv.command, msg_recv.arg_1, msg_recv.arg_2);
			if(client_message_handler&&!(msg_recv.command&MSG_PAYLOAD_BIT))
				client_message_handler(&msg_recv, NULL);
		}
	}
	
	for(i=0; recalc_map; recalc_map>>=1, i++)
		if(recalc_map&1) {
			if(i==map->layers-1)
				map_calculate_powergrid();
			darnitRenderTilemapRecalculate(map->layer[i].tilemap);
			map_minimap_update();
		}
	return;
}

void client_game_handler(MESSAGE_RAW *msg, unsigned char *payload) {
	switch(msg->command) {
		PONG;
		case MSG_RECV_CHAT:
			chat_recv(msg->player_id, (char *)payload, msg->arg_2);
			break;
		case MSG_RECV_LEAVE:
			chat_leave(msg->player_id);
			break;
		case MSG_RECV_MAP_TILE_ATTRIB:
			map_set_tile_attributes(msg->arg_2, msg->arg_1);
			recalc_map|=1<<(map->layers-1);
			break;
		case MSG_RECV_BUILDING_PLACE:
			map_building_place(msg->arg_2, msg->player_id, msg->arg_1);
			recalc_map|=1<<(map->layers-2);
			if(msg->player_id==player_id&&msg->arg_1) {
				if(msg->arg_1==BUILDING_GENERATOR) {
					map_set_home(msg->arg_2);
					game_view_scroll_to(home_x, home_y);
				}
				game_reset_building_progress();
			}
			break;
		case MSG_RECV_BUILDING_PROGRESS:
			game_set_building_progress(0, msg->arg_2);
			break;
		case MSG_RECV_BUILDING_HP:
			map_set_building_health(msg->arg_2, msg->arg_1);
			break;
		case MSG_RECV_BUILDING_SHIELD:
			map_set_building_shield(msg->arg_2, msg->arg_1);
			break;
	}
}

void client_countdown_handler(MESSAGE_RAW *msg, unsigned char *payload) {
	switch(msg->command) {
		PONG;
		case MSG_RECV_CHAT:
			chat_recv(msg->player_id, (char *)payload, msg->arg_2);
			break;
		case MSG_RECV_LEAVE:
			chat_leave(msg->player_id);
			break;
		case MSG_RECV_GAME_START:
			chat_countdown(msg->arg_1);
			if(!msg->arg_1) {
				lobby_ready_checkbox_disable();
				game_state(GAME_STATE_GAME);
				client_message_handler=client_game_handler;
			}
			break;
	}
}

void client_download_map(MESSAGE_RAW *msg, unsigned char *payload) {
	static int filesize_bytes=0, downloaded_bytes=0;
	static char *filename=NULL;
	static DARNIT_FILE *f=NULL;
	switch(msg->command) {
		PONG;
		case MSG_RECV_CHAT:
			chat_recv(msg->player_id, (char *)payload, msg->arg_2);
			break;
		case MSG_RECV_JOIN:
			if(!payload)
				break;
			memcpy(&player_names[msg->player_id*32], payload, msg->arg_2);
			chat_join(msg->player_id);
			break;
		case MSG_RECV_LEAVE:
			chat_leave(msg->player_id);
			break;
		case MSG_RECV_MAP_BEGIN:
			if(!payload)
				break;
			map_close(map);
			if(filename) {
				darnitFSUnmount(filename);
				free(filename);
				darnitFileClose(f);
			}
			filesize_bytes=msg->arg_1;
			filename=malloc(msg->arg_2+1);
			memcpy(filename, payload, msg->arg_2);
			filename[msg->arg_2]=0;
			f=darnitFileOpen(filename, "wb");
			break;
		case MSG_RECV_MAP_CHUNK:
			if(!payload)
				break;
			downloaded_bytes+=msg->arg_2;
			darnitFileWrite(payload, msg->arg_2, f);
			client_message_send(player_id, MSG_SEND_READY, 0, 100*downloaded_bytes/filesize_bytes, NULL);
			lobby_set_map_progress(100*downloaded_bytes/filesize_bytes);
			break;
		case MSG_RECV_MAP_END:
			darnitFileClose(f);
			f=NULL;
			client_message_send(player_id, MSG_SEND_READY, 0, 100, NULL);
			darnitFSMount(filename);
			map_init("maps/map.ldmz");
			lobby_ready_checkbox_enable();
			break;
		case MSG_RECV_GAME_START:
			game_state(GAME_STATE_LOBBY);
			client_message_handler=client_countdown_handler;
			client_countdown_handler(msg, payload);
	}
}

void client_identify(MESSAGE_RAW *msg, unsigned char *payload) {
	player_id=msg->player_id;
	player_names=(char *)calloc(msg->arg_2, 32);
	player_name[31]=0;
	client_message_send(player_id, MSG_SEND_IDENTIFY, API_VERSION, strlen(player_name), player_name);
	client_message_handler=client_download_map;
}

int client_init(char *host, int port) {
	player_names=NULL;
	if((sock=darnitSocketConnect(host, port, client_connect_callback, NULL))==NULL)
		return -1;
	msg_recv.command=0;
	msg_recv_payload_offset=msg_recv_payload;
	msg_recv_offset=&msg_recv;
	client_message_handler=client_identify;
	chat_clear();
	return 0;
}

void client_disconnect() {
	client_connect_callback(-1, NULL, sock);
	sock=NULL;
}
