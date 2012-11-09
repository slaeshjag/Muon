#ifndef __UNIT_H__
#define	__UNIT_H__

#define	UNITS_DEFINED			6

#define	UNIT_DEF_NOTHING		0
#define	UNIT_DEF_GENERATOR		1
#define	UNIT_DEF_SCOUT			2
#define	UNIT_DEF_ATTACKER		3
#define	UNIT_DEF_PYLON			4
#define	UNIT_DEF_WALL			5
#define	UNIT_DEF_BUILDSITE		6
#define	UNIT_DEF_BUILDSITE_FREE		0x100

#define	UNIT_BUILDSITE			5

#define	UNIT_ATTACKER_DMGP		5
#define	UNIT_REGEN_DELAY		5000

static const int unit_los[] = 		{ 0,          /* nothing */
	                                  7,          /* generator */
	                                  6,          /* scout */
	                                  3,          /* attacker */
	                                  4,          /* pylon */
	                                  1,          /* wall */
	                                  7           /* owned control point (buildsite) */
	                                };

/* 750 is the base coefficient for hp,
 * to make numbers easier to work with.
 * think of the non-multiplied number as
 * how many tenths of a second it takes
 * to kill a building with a given number
 * of attacking units, when the building
 * is unpowered. */
static const int unit_maxhp[] = 	{ 750 * 0,       /* nothing */
	                                  750 * 150,     /* generator */
	                                  750 * 50,      /* scout */
	                                  750 * 90,      /* attacker */
	                                  750 * 40,      /* pylon */
	                                  750 * 10,      /* wall */
	                                  750 * 50       /* owned control point (buildsite) */
	                                };

/* Shield must not be 0 for any existing building! */
/* same coefficient as hp, the exception being
 * buildings that "doesn't have" shields. */
static const int unit_maxshield[] =  { 0,              /* nothing */
	                                   750 * 150,    /* generator */
	                                   1,              /* scout */
	                                   750 * 60,     /* attacker */
	                                   750 * 30,     /* pylon */
	                                   750 * 90,     /* wall */
	                                   1               /* owned control point (buildsite) */
	                                 };

static const int unit_shieldreg[] =  { 0,          /* nothing */
	                                   1,          /* generator */
	                                   1,          /* scout */
	                                   1,          /* attacker */
	                                   1,          /* pylon */
	                                   1,          /* wall */
	                                   1           /* owned control point (buildsite) */
	                                 };

/* Meant to replace UNIT_ATTACKER_DMGP */
static const int unit_damage[] =     { 0,          /* nothing */
	                                   0,          /* generator */
	                                   1,          /* scout */
	                                   5,          /* attacker */
	                                   0,          /* pylon */
	                                   0,          /* wall */
	                                   0           /* owned control point (buildsite) */
	                                 };

/* Pylons and generators must have the same distribution range... */
/* range is for shooting and power distribution, whereever
 * applicable. units that don't do either have a range of 0. */
static const int unit_range[] = 	 { 0,          /* nothing */
                                       4,          /* generator, distribution range */
                                       4,          /* scout, attack range */
                                       5,          /* attacker, attack range */
                                       4,          /* pylon, redistribution range */
                                       0,          /* wall */
                                       0           /* owned control point (buildsite) */
                                     };

/* measured in ticks, of which there are 3000/s with normal game speed. */
static const int unit_buildtime[] =	 { 0,          /* nothing */
	                                   0,          /* generator */
	                                   5000,       /* scout */
	                                   20000,      /* attacker */
	                                   10000,      /* pylon */
	                                   10000,      /* wall */
	                                   10000       /* owned control point (buildsite) */
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
int unitSpawn(unsigned int player, unsigned int unit, unsigned int x, unsigned int y);
int unitAdd(int owner, int type, int x, int y);
int unitRemove(int x, int y);
void unitDestroy(int player, unsigned int index);
void unitPylonPulse();

void unitDestroyAll(int player);
void unitLoop(int msec);

int unitAttackValidate(int index_src, int owner, int index_dst);
void unitAttackSet(int index_src, int index_dst);

#endif
