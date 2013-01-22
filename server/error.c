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


const char *server_error_pop() {
	return errorPop();
}
