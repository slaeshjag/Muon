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

#ifndef __UNIT_H__
#define	__UNIT_H__

#define	UNITS_DEFINED			8

#define	UNIT_DEF_NOTHING		0
#define	UNIT_DEF_GENERATOR		1
#define	UNIT_DEF_SCOUT			2
#define	UNIT_DEF_ATTACKER		3
#define	UNIT_DEF_PYLON			4
#define	UNIT_DEF_WALL			5

/* These can be build on an empty control point */
#define	UNIT_DEF_BUILDSITE		6
#define	UNIT_DEF_CLUSTERBOMB		7
#define	UNIT_DEF_RADAR			8

#define	UNIT_DEF_BUILDSITE_FREE		0x100

#define	UNIT_BUILDSITE			5

#define	UNIT_ATTACKER_DMGP		5
#define	UNIT_REGEN_DELAY		5000

static const int unit_los[] = {
	0,		/* nothing */
	7,		/* generator */
	6,		/* scout */
	3,		/* attacker */
	4,		/* pylon */
	0,		/* wall */
	7,		/* owned control point (buildsite) */
	2,		/* carpet bomb */
	2		/* radar */
};

/* 2000 is the base coefficient for hp,
 * to make numbers easier to work with.
 * think of the non-multiplied number as
 * how many tenths of a second it takes
 * to kill a building with a given number
 * of attacking units, when the building
 * is unpowered. */
static const int unit_maxhp[] = {
	2000 * 0,       /* nothing */
	2000 * 150,     /* generator */
	2000 * 75,      /* scout */
	2000 * 90,      /* attacker */
	2000 * 40,      /* pylon */
	1,              /* wall */
	2000 * 50,	/* owned control point (buildsite) */
	2000 * 50,	/* carpet bomb */
	2000 * 30	/* radar */
};

/* Shield must not be 0 for any existing building! */
/* same coefficient as hp, the exception being
 * buildings that "doesn't have" shields. */
static const int unit_maxshield[] = {
	0,		/* nothing */
	2000 * 150,	/* generator */
	1,		/* scout */
	2000 * 90,	/* attacker */
	2000 * 30,	/* pylon */
	2000 * 200,	/* wall */
	1,		/* owned control point (buildsite) */
	1,		/* carpet bomb */
	1		/* radar */
};

static const int unit_shieldreg[] = {
	0,		/* nothing */
	1,		/* generator */
	1,		/* scout */
	1,		/* attacker */
	1,		/* pylon */
	1,		/* wall */
	1,		/* owned control point (buildsite) */
	1,		/* carpet bomb */
	1,		/* radar */
};

/* Meant to replace UNIT_ATTACKER_DMGP */
static const int unit_damage[] = {
	0,		/* nothing */
	0,		/* generator */
	2,		/* scout */
	5,		/* attacker */
	0,		/* pylon */
	0,		/* wall */
	0,		/* owned control point (buildsite) */
	2000 * 150 * 50,/* carpet bomb (this / Generator shield gives bomb count) */
	0		/* radar */
};

#define	UNIT_NUKE_MAX_DEFLECTION	2000 * 75

/* Pylons and generators must have the same distribution range... */
/* range is for shooting and power distribution, whereever
 * applicable. units that don't do either have a range of 0. */
static const int unit_range[] = {
	0,		/* nothing */
	4,		/* generator, distribution range */
	4,		/* scout, attack range */
	5,		/* attacker, attack range */
	4,		/* pylon, redistribution range */
	0,		/* wall */
	0,		/* owned control point (buildsite) */
	7,		/* carpet bomb (radius of spread) */
	7,		/* Radius of the FoW "window" that opens */
};

/* measured in ticks, of which there are 3000/s with normal game speed. */
static const int unit_buildtime[] = {
	0,		/* nothing */
	0,		/* generator */
	8000,		/* scout */
	60000,		/* attacker */
	20000,		/* pylon */
	15000,		/* wall */
	50000,		/* owned control point (buildsite) */
	100000,		/* carpet bomb */
	70000		/* radar */
};

/* Points for determening a winner without generators being lost. */
/* this is really just based off of the cost of building them,
 * normalised against the scout... */
static const int unit_points[] = { 
	1,		/* Powered tile */
	0,		/* Generator */
	1,		/* Scout */
	8,		/* Attacker */
	0,		/* Pylon */
	2,		/* Wall */
	6,		/* Owned control point */
	13,		/* carpet bomb */
	9		/* radar */
};

struct SERVER_UNIT;


struct UNIT_PYLON;

typedef struct UNIT_PYLON {
	unsigned int			power	: 1;
	unsigned int			pulse	: 1;
	unsigned int			x;
	unsigned int			y;
	struct SERVER_UNIT		*unit;
	struct UNIT_PYLON		**neighbour;
	unsigned int			neighbours;
	struct UNIT_PYLON		*next;			/* Only used to keep track of 'em all */
} UNIT_PYLON;


typedef struct {
	unsigned int		max_hp;
	unsigned int		max_shield;
} UNIT_DEF;

int unitLOS(int unit);
int unitMaxHP(int unit);
int unitShieldMax(int type);
int unitRange(int type);
int unitValidateWall(int index, int player);
void unitAnnounce(int from, int to, int building, int index);
void unitDamageDo(int index, int damage, int time);
void unitDamagePoke(int index, int damage);
int unitSpawn(unsigned int player, unsigned int unit, unsigned int x, unsigned int y);
int unitAdd(int owner, int type, int x, int y);
int unitRemove(int x, int y);
void unitDestroy(int player, unsigned int index);
void unitPylonPulse();

void unitDestroyAll(int player);
void unitLoop(int msec);

int unitAttackValidate(int index_src, int owner, int index_dst);
void unitAttackSet(int index_src, int index_dst);
void unitAnnounceBuildingData(int player);

#endif
