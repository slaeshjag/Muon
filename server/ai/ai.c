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

#include <stdlib.h>
#include <time.h>

#include "ai.h"
#include "scout.h"

static AI *ai=NULL;
static int ais;

void ai_init() {
	personality[PERSONALITY_SCOUT].task.idle=scout_idle;
	personality[PERSONALITY_SCOUT].task.spot=NULL;
	personality[PERSONALITY_SCOUT].task.engage=NULL;
}

void ai_join(int n) {
	int i;
	srand(time(NULL));
	ai=calloc(sizeof(AI), n);
	ais=n;
	for(i=0; i<ais; i++)
		ai[i].personality=rand()%PERSONALITIES;
}

void ai_loop() {
	int i;
	if(!ai)
		return;
	for(i=0; i<ais; i++)
		personality[ai[i].personality].task.idle(&ai[i]);
}
