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
#include <string.h>
#include <limits.h>

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
		free(player);
		player=NULL;
		if(ret==-1)
			game_state(GAME_STATE_MENU);
		sock=d_socket_close(socket);
		player_id=0;
		if(!server_is_running())
			server_stop();
		map_close();
	} else
		game_state(GAME_STATE_LOBBY);
}

void client_message_convert_send(MESSAGE_RAW *message) {
	message->player_id=d_util_htonl(message->player_id);
	message->command=d_util_htonl(message->command);
	message->arg_1=d_util_htonl(message->arg_1);
	message->arg_2=d_util_htonl(message->arg_2);
}

void client_message_convert_recv(MESSAGE_RAW *message) {
	message->player_id=d_util_ntohl(message->player_id);
	message->command=d_util_ntohl(message->command);
	message->arg_1=d_util_ntohl(message->arg_1);
	message->arg_2=d_util_ntohl(message->arg_2);
}

void client_message_send(int player_id, int command, int arg_1, int arg_2, char *payload) {
	MESSAGE_RAW msg_send;
	msg_send.player_id=player_id;
	msg_send.command=command;
	msg_send.arg_1=arg_1;
	msg_send.arg_2=arg_2;
	client_message_convert_send(&msg_send);
	d_socket_send(sock, &msg_send, sizeof(MESSAGE_RAW));
	if(payload)
		d_socket_send(sock, payload, arg_2);
}

void client_check_incoming() {
	int s, i;
	unsigned int chunk_got=0, chunk_size, chunk_time;
	static unsigned int chunk_size_time=UINT_MAX;
	if((s=d_socket_recv_try(sock, &chunk_size, 4))<4) {
		if(s==-1)
			client_disconnect(MSG_SERVER_DISCONNECT);
		if(d_time_get()-chunk_size_time>CLIENT_TIMEOUT) {
			client_disconnect(MSG_SERVER_DISCONNECT);
			return;
		} else
			return;
	} else
		chunk_size_time=d_time_get();
	chunk_size=d_util_htonl(chunk_size)-4;
	chunk_time=d_time_get();
	
	while(chunk_got<chunk_size) {
		if(d_time_get()-chunk_time>CLIENT_TIMEOUT) {
			client_disconnect(MSG_SERVER_DISCONNECT);
			return;
		}
		if(msg_recv.command&MSG_PAYLOAD_BIT) {
			//download message payload
			//printf("payload size %i\n", msg_recv.arg_2);
			s=d_socket_recv_try(sock, msg_recv_payload, msg_recv.arg_2);
			if(s==0)
				continue;
			if(s==-1) {
				client_disconnect(MSG_SERVER_DISCONNECT);
				return;
			}
			if(client_message_handler)
				if(client_message_handler(&msg_recv, msg_recv_payload)) {
					chunk_size_time=UINT_MAX;
					return;
				}
			msg_recv.command=0;
		} else {
			//download message
			s=d_socket_recv_try(sock, msg_recv_offset, sizeof(MESSAGE_RAW));
			if(s==0) {
				msg_recv.command = 0;
				continue;
			}
			if(s==-1) {
				client_disconnect(MSG_SERVER_DISCONNECT);
				return;
			}
			client_message_convert_recv(&msg_recv);
			//printf("message: 0x%x (%i, %i) to player %i\n", msg_recv.command, msg_recv.arg_1, msg_recv.arg_2, msg_recv.player_id);
			if(client_message_handler&&!(msg_recv.command&MSG_PAYLOAD_BIT))
				if(client_message_handler(&msg_recv, NULL)) {
					chunk_size_time=UINT_MAX;
					return;
				}
		}
		chunk_got+=s;
	}
	client_message_send(player_id, MSG_SEND_CHUNK_OK, 0, 0, NULL);
	
	//TODO: move somewhere else
	if(recalc_map) {
		UI_PROPERTY_VALUE v;
		v=game_sidebar_minimap->get_prop(game_sidebar_minimap, UI_IMAGEVIEW_PROP_TILESHEET);
		map_minimap_update(v.p, game_sidebar_minimap->w, game_sidebar_minimap->h, 1);
		if(recalc_map&~(3<<(map->layers-2)))
			map_update_grid();
	}
	for(i=0; recalc_map; recalc_map>>=1, i++)
		if(recalc_map&1) {
			if(i==map->layers-1)
				map_calculate_powergrid();
			d_tilemap_recalc(map->layer[i].tilemap);
		}
	return;
}

int client_game_handler(MESSAGE_RAW *msg, unsigned char *payload) {
	switch(msg->command) {
		PONG;
		case MSG_RECV_KICKED:
			client_disconnect(msg->command);
			break;
		case MSG_RECV_CHAT:
			chat_recv(msg->player_id, msg->arg_1, (char *)payload, msg->arg_2);
			sound_play(sound_chat);
			break;
		case MSG_RECV_LEAVE:
			chat_leave(msg->player_id);
			break;
		case MSG_RECV_PLAYER_DEFEATED:
			chat_defeated(msg->player_id);
			sound_play(sound_defeated);
			break;
		case MSG_RECV_CLEAR_MAP:
			map_clear_fow();
			break;
		case MSG_RECV_BUILDING_ATTACK:
			game_attacklist_target(msg->arg_2, msg->arg_1);
			break;
		case MSG_RECV_MAP_TILE_ATTRIB:
			recalc_map|=map_set_tile_attributes(msg->arg_2, msg->arg_1);
			break;
		case MSG_RECV_BUILDING_PLACE:
			map_building_place(msg->arg_2, msg->player_id, msg->arg_1);
			recalc_map|=1<<(map->layers-2);
			if(msg->player_id==player_id&&msg->arg_1) {
				if(msg->arg_1==BUILDING_GENERATOR&&!map_isset_home()) {
					map_set_home(msg->arg_2);
					game_view_scroll_to(home_x, home_y);
				}
				if(msg->arg_1>=BUILDING_BUILDSITE)
					panelist_game_sidebar.next=&panelist_game_abilitybar;
				if(msg->arg_1!=BUILDING_NONE) {
					game_reset_building_progress();
					game_set_building_ready(BUILDING_NONE);
			}
			}
			break;
		case MSG_RECV_UNIT_READY:
			game_set_building_ready(msg->arg_1);
			sound_play(sound_ready);
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
		case MSG_RECV_MAP_FLARE:
			map_flare_add(msg->arg_2, msg->player_id, 10000, 40);
			sound_play(sound_flare);
			break;
		case MSG_RECV_CP_TIMER: {
			int i=msg->arg_1-BUILDING_CLUSTERBOMB+1;
			char s[5];
			ability[i].ready=msg->arg_2;
			d_text_surface_reset(ability[i].text);
			sprintf(s, "%i%%", ability[i].ready);
			d_text_surface_offset_next_set(ability[i].text, ability[i].button->w/2-d_font_string_w(font_std, s)/2);
			d_text_surface_string_append(ability[i].text, s);
			
			if(msg->arg_2==100)
				ability[i].button->enabled=1;
			else
				ability[i].button->enabled=0;
			//printf("cp timer\n");
			break;
		}
		case MSG_RECV_CP_CLEAR:
			ability[msg->arg_1-BUILDING_CLUSTERBOMB+1].ready=-1;
			ability[msg->arg_1-BUILDING_CLUSTERBOMB+1].button->enabled=0;
			//printf("cp clear\n");
			break;
		case MSG_RECV_CP_DEPLOY:
			if(msg->arg_1==BUILDING_CLUSTERBOMB) {
				map_flare_add(msg->arg_2, msg->player_id, 2000, building[BUILDING_CLUSTERBOMB].range*map->layer[map->layers-2].tile_w);
				/*if(msg->player_id==player_id) {
					ability[1].ready=-1;
					ability[1].button->enabled=0;
				}*/
			} else if(msg->arg_1==BUILDING_RADAR)
				sound_play(sound_radar);
			break;
		case MSG_RECV_MAJOR_IMPACT:
			map_flare_add(msg->arg_2, msg->player_id, 2000, map->layer[map->layers-2].tile_w);
			sound_play(sound_nuke);
			break;
		case MSG_RECV_PLAYER_STATS_1:
			player[msg->player_id].stats.built=msg->arg_1;
			player[msg->player_id].stats.lost=msg->arg_2;
			break;
		case MSG_RECV_PLAYER_STATS_2:
			player[msg->player_id].stats.destroyed=msg->arg_1;
			player[msg->player_id].stats.efficiency=msg->arg_2;
			break;
		case MSG_RECV_PLAYER_POINTS:
			player[msg->player_id].stats.score=msg->arg_1;
			break;
		case MSG_RECV_GAME_ENDED:
			//ui_messagebox(font_std, T("Game over"));
			game_state(GAME_STATE_GAME_OVER);
			recalc_map=0;
			return -1;
	}
	return 0;
}

int client_download_map(MESSAGE_RAW *msg, unsigned char *payload) {
	static int filesize_bytes=0, downloaded_bytes=0;
	static char *filename=NULL;
	static DARNIT_FILE *f=NULL;
	switch(msg->command) {
		PONG;
		case MSG_RECV_KICKED:
		case MSG_RECV_NAME_IN_USE:
		case MSG_RECV_SERVER_FULL:
		case MSG_RECV_BAD_CLIENT:
			client_disconnect(msg->command);
			break;
		case MSG_RECV_PLAYER_READY:
			if(msg->arg_2==100) {
				if(msg->player_id==player_id)
					lobby_download_complete();
				player[msg->player_id].ready=msg->arg_1;
				lobby_update_player(msg->player_id);
			} else
				lobby_progress(msg->player_id, msg->arg_2);
			break;
		case MSG_RECV_CHAT:
			chat_recv(msg->player_id, msg->arg_1, (char *)payload, msg->arg_2);
			break;
		case MSG_RECV_JOIN:
			if(!payload)
				break;
			memcpy(&player[msg->player_id].name, payload, msg->arg_2);
			player[msg->player_id].name[msg->arg_2]=0;
			lobby_join(msg->player_id);
			break;
		case MSG_RECV_LEAVE:
			lobby_leave(msg->player_id);
			break;
		case MSG_RECV_MAP_BEGIN:
			if(!payload)
				break;
			map_close();
			if(filename) {
				d_fs_unmount(filename);
				free(filename);
				d_file_close(f);
			}
			filesize_bytes=msg->arg_1;
			filename=malloc(5+msg->arg_2+1);
			payload[msg->arg_2]=0;
			sprintf(filename, "%s/%s", mapdir, payload);
			f=d_file_open(filename, "wb");
			break;
		case MSG_RECV_MAP_CHUNK:
			if(!payload)
				break;
			downloaded_bytes+=msg->arg_2;
			d_file_write(payload, msg->arg_2, f);
			client_message_send(player_id, MSG_SEND_READY, 0, 100*downloaded_bytes/filesize_bytes, NULL);
			lobby_set_map_progress(100*downloaded_bytes/filesize_bytes);
			break;
		case MSG_RECV_MAP_END:
			d_file_close(f);
			f=NULL;
			client_message_send(player_id, MSG_SEND_READY, 0, 100, NULL);
			d_fs_mount(filename);
			map_init("mapdata/map.ldmz");
			lobby_map_preview_generate();
			map_building_clear();
			break;
		case MSG_RECV_GAME_START:
			chat_countdown(msg->arg_1);
			if(!msg->arg_1) {
				chat_hide(&panelist_game_sidebar);
				game_time_start=d_time_get();
				game_state(GAME_STATE_GAME);
				client_message_handler=client_game_handler;
			}
			break;
		case MSG_RECV_PLAYER_INFO:
			player[msg->player_id].team=msg->arg_1;
			lobby_update_player(msg->player_id);
			break;
		case MSG_RECV_UNIT_RANGE:
			building[msg->arg_1].range=msg->arg_2;
			break;
	}
	return 0;
}

int client_identify(MESSAGE_RAW *msg, unsigned char *payload) {
	player_id=msg->player_id;
	players=msg->arg_2;
	player=calloc(players, sizeof(PLAYER));
	config.player_name[31]=0;
	client_message_send(player_id, MSG_SEND_IDENTIFY, API_VERSION, strlen(config.player_name), config.player_name);
	client_message_handler=client_download_map;
	lobby_open();
	if(!server_is_running())
		server_admin_set(player_id);
	return 0;
}

int client_init(char *host, int port) {
	player=NULL;
	if((sock=d_socket_connect(host, port, client_connect_callback, NULL))==NULL)
		return -1;
	msg_recv.command=0;
	msg_recv_payload_offset=msg_recv_payload;
	msg_recv_offset=&msg_recv;
	client_message_handler=client_identify;
	chat_clear();
	return 0;
}

void client_disconnect(int cause) {
	switch(cause) {
		case MSG_RECV_KICKED:
			ui_messagebox(font_std, T("You were kicked from the server!"));
			break;
		case MSG_RECV_NAME_IN_USE:
			ui_messagebox(font_std, T("Your name is already in use, please change it in the settings."));
			break;
		case MSG_RECV_SERVER_FULL:
			ui_messagebox(font_std, T("The server is full."));
			break;
		case MSG_RECV_BAD_CLIENT:
			ui_messagebox(font_std, T("You are using an outdated client, please update Muon."));
			break;
		case MSG_SERVER_DISCONNECT:
			ui_messagebox(font_std, T("Lost connection to server."));
			break;
	}
	client_connect_callback(cause==MSG_RECV_GAME_ENDED?-2:-1, NULL, sock);
	sock=NULL;
}
