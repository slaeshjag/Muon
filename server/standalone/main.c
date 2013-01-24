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
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#else
#include <time.h>
#include <windows.h>
#endif

#include "server.h"

struct timeval time_d;


int deltaTime() {
	struct timeval time_now;
	int delta;

	gettimeofday(&time_now, NULL);
	delta = time_now.tv_usec - time_d.tv_usec;
	delta /= 1000;
	delta += (time_now.tv_sec - time_d.tv_sec) * 1000;
	time_d = time_now;

	return delta;
}


int main(int argc, char **argv) {
	int port, gamespeed;

	if (argc < 4) {
		fprintf(stdout, "Usage: %s <map> <gamespeed (1-10)> <players> [port]\n", argv[0]);
		return -1;
	}

	if (argc >= 5)
		port = atoi(argv[4]);
	else
		port = SERVER_PORT_DEFAULT;
	
	server_init();
	gettimeofday(&time_d, NULL);
	gamespeed = atoi(argv[2]);

	if (server_start(argv[1], atoi(argv[3]), port, gamespeed) == NULL)
		return -1;
	for (;;) {
		server_loop(deltaTime());
		#ifdef _WIN32
		Sleep(15);
		#else
		usleep(15000);
		#endif
	}

	
	return 0;
}
