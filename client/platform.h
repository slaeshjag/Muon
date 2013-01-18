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

#ifndef PLATFORM_H
#define PLATFORM_H

#define T(s) darnitStringtableEntryGet(stringtable, (s))

DARNIT_PLATFORM platform;
DARNIT_STRINGTABLE *stringtable;

struct CONFIG {
	int screen_w;
	int screen_h;
	int fullscreen;
	int grid;
	int powergrid;
	int plasma;
	int alpha;
	int gamespeed;
	int players;
	char player_name[32];
	char lang[3];
} config;

struct CONFIG_PARSER {
	const char *option;
	void (*parser_func)(void *opt, char *p);
	void (*writer_func)(void *opt, char *p);
	void *config_opt;
};

DARNIT_VIDEOMODE **videomodes;

void platform_init();

void platform_config_read();
void platform_config_write();

void platform_option_read_int(void *opt, char *p);
void platform_option_read_string(void *opt, char *p);
void platform_option_write_int(void *opt, char *p);
void platform_option_write_string(void *opt, char *p);

#endif
