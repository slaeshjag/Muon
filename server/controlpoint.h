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

#ifndef __CONTROLPOINT_H__
#define	__CONTROLPOINT_H__

#define	CONTROLPOINT_CLUSTERBOMB_WARMUP	10800

/* In gameticks */
#define	CP_CLUSTERBOMB_DELAY		60 * 180

typedef struct CONTROLPONIT_EXTRA {
	unsigned int			type;
	unsigned int			delay;
	unsigned int			index;
	unsigned int			owner;
	struct CONTROLPOINT_EXTRA	*next;
} CONTROLPOINT_EXTRA;


struct SERVER_UNIT;

int controlpointInit();
int controlpointNew(struct SERVER_UNIT *unit);
void controlpointLoop();
void controlpointRemove(struct SERVER_UNIT *unit);

#endif
