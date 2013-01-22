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

#define	CP_DELAY_SEC(x)			((x) / server->game.gamespeed / 1000)

#define	CP_BUILDSITE_SPEED(x)		(powf(1.3, (x)))
#define	CP_RADAR_SPEED(x)		((x))
#define	CP_CLUSTERBOMB_SPEED(x)		((x))
#define	CP_GROUNDGEN_SPEED(x)		((x))
#define	CP_SHIELDREGEN_SPEED(x)		((x))

/* In gameticks */
#define	CP_CLUSTERBOMB_DELAY		180 * 3000
#define	CP_RADAR_DELAY			90 * 3000
#define	CP_RADAR_DEPLOY_TIME		10 * 3000
#define	CP_GROUNDGEN_DELAY		30 * 3000
#define	CP_SHIELDREGEN_DELAY		60 * 3000

struct SERVER_UNIT;

typedef struct CONTROLPONIT_EXTRA {
	unsigned int			type;
	unsigned int			index;
	unsigned int			owner;
	struct CONTROLPOINT_EXTRA	*next;
} CONTROLPOINT_EXTRA;


typedef struct {
	int				count;
	int				temp;
	float				speed;
} CONTROLPOINT_ENTRY;


typedef struct {
	int				clusterbomb_delay;
	int				radar_delay;
	int				radar_deploy;
	int				radar_pos;
	int				groundgen_delay;
	int				shieldregen_delay;

	CONTROLPOINT_ENTRY		clusterbomb;
	CONTROLPOINT_ENTRY		radar;
	CONTROLPOINT_ENTRY		groundgen;
	CONTROLPOINT_ENTRY		shieldregen;
} CONTROLPOINT_DATA;



int controlpointInit();
void controlpointInitPlayer(int player);
int controlpointNew(struct SERVER_UNIT *unit);
void controlpointLoop();
void controlpointDeploy(int player, int type, int index_dst);
void controlpointRemove(struct SERVER_UNIT *unit);

#endif
