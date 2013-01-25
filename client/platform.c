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

#include <string.h>

#include "muon.h"
#include "client.h"
#include "platform.h"

struct CONFIG_PARSER parsers[]={
	{"width", platform_option_read_int, platform_option_write_int, &config.screen_w},
	{"height", platform_option_read_int, platform_option_write_int, &config.screen_h},
	{"fullscreen", platform_option_read_int, platform_option_write_int, &config.fullscreen},
	{"grid", platform_option_read_int, platform_option_write_int, &config.grid},
	{"powergrid", platform_option_read_int, platform_option_write_int, &config.powergrid},
	{"plasma", platform_option_read_int, platform_option_write_int, &config.plasma},
	{"alpha", platform_option_read_int, platform_option_write_int, &config.alpha},
	{"gamespeed", platform_option_read_int, platform_option_write_int, &config.gamespeed},
	{"players", platform_option_read_int, platform_option_write_int, &config.players},
	{"sound", platform_option_read_int, platform_option_write_int, &config.sound},
	{"music", platform_option_read_int, platform_option_write_int, &config.music},
	{"lang", platform_option_read_string, platform_option_write_string, &config.lang},
	{"name", platform_option_read_string, platform_option_write_string, &config.player_name},
};


void platform_option_read_int(void *opt, char *p) {
	*((int *)opt)=atoi(p);
}

void platform_option_read_string(void *opt, char *p) {
	strcpy((char *)opt, p);
}

void platform_option_write_int(void *opt, char *p) {
	sprintf(p, "%i", *((int *)opt));
}

void platform_option_write_string(void *opt, char *p) {
	strcpy(p, (char *)opt);
}

void platform_config_init_defaults() {
	if(platform.platform&DARNIT_PLATFORM_HANDHELD) {
		config.screen_w=800;
		config.screen_h=480;
		config.fullscreen=1;
		if(platform.platform&DARNIT_PLATFORM_MAEMO) {
			DARNIT_INPUT_MAP keymap={
				KEY(w),
				KEY(s),
				KEY(a),
				KEY(d),
				KEY(KP_ENTER),
				KEY(LSHIFT),
				KEY(F4),
				KEY(r),
				KEY(BACKSPACE),
				KEY(LCTRL),
				KEY(F1),
				KEY(F2),
			};
		d_keymapping_set(keymap);
		}
	} else {
		config.screen_w=800;
		config.screen_h=600;
		config.fullscreen=0;
		DARNIT_INPUT_MAP keymap={
			KEY(w),
			KEY(s),
			KEY(a),
			KEY(d),
			KEY(TAB),
			KEY(LSHIFT),
			KEY(SPACE),
			KEY(DELETE),
			KEY(ESCAPE),
			KEY(RETURN),
			KEY(F1),
			KEY(F2),
		};
		d_keymapping_set(keymap);
	}
	config.grid=1;
	config.powergrid=0;
	config.plasma=3;
	config.alpha=1;
	config.gamespeed=CLIENT_DEFAULT_GAMESPEED;
	config.players=CLIENT_DEFAULT_PLAYERS;
	config.sound=7;
	config.music=5;
	strcpy(config.lang, "EN");
	strcpy(config.player_name, "player");
}

void platform_config_read() {
	char buf[128];
	int len, i;
	DARNIT_FILE *f;
	if(!(f=d_file_open("muon.cfg", "rb"))) {
		platform_config_write();
		return;
	}
	while((len=d_file_getln(buf, 128, f))) {
		if(buf[0]=='#')
			continue;
		for(i=0; i<sizeof(parsers)/sizeof(struct CONFIG_PARSER); i++) {
			int optlen=strlen(parsers[i].option);
			if(memcmp(buf, parsers[i].option, optlen)||buf[optlen]!='=')
				continue;
			buf[len]=0;
			parsers[i].parser_func(parsers[i].config_opt, &buf[optlen+1]);
			break;
		}
	}
	d_file_close(f);
}

void platform_config_write() {
	char buf[128];
	int i;
	DARNIT_FILE *f;
	if(!(f=d_file_open("muon.cfg", "wb")))
		return;
	d_file_write("#Muon\n", 6, f);
	for(i=0; i<sizeof(parsers)/sizeof(struct CONFIG_PARSER); i++) {
		strcpy(buf, parsers[i].option);
		strcat(buf, "=");
		parsers[i].writer_func(parsers[i].config_opt, &buf[strlen(buf)]);
		strcat(buf, "\n");
		d_file_write(buf, strlen(buf), f);
	}
	d_file_close(f);
}

/*void platform_config_write_option(enum CONFIG_OPTIONS opt, void *value) {
	char buf[128];
	int i;
	DARNIT_FILE *f;
	if(!(f=d_file_open("muon.cfg", "r+")))
		return;
	while((len=d_file_getln(buf, 128, f))) {
		if(buf[0]=='#')
			continue;
		
		int optlen=strlen(parsers[opt].option);
		if(memcmp(buf, parsers[opt].option, optlen)||buf[optlen]!='=')
			continue;
		buf[len]=0;
		parsers[i].parser_func(parsers[i].config_opt, &buf[optlen+1]);
	}
	d_file_close(f);
}*/

void platform_init() {
	d_init_partial("muon");
	platform=d_platform_get();
	videomodes=d_videomode_get();
	platform_config_init_defaults();
	platform_config_read();
	d_init_rest("Muon", config.screen_w, config.screen_h, config.fullscreen, "res/icon.png");
	platform=d_platform_get();
	d_directory_create("maps");
	stringtable=d_stringtable_open("res/lang.stz");
	if(d_stringtable_section_load(stringtable, config.lang)==-1)
		d_stringtable_section_load(stringtable, "EN");
	d_input_unicode(1);
}
