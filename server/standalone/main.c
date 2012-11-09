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
		port = SERVER_PORT_DEFAULT;
	
	serverInit();

	if (serverStart(argv[1], atoi(argv[2]), port, 10) == NULL)
		return -1;
	for (;;) {
		sleep(1);
		serverLoop(1000);
	}

	
	return 0;
}
