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

static const int unit_los[] = 		{ 0, 7, 6, 3, 4, 1, 2};
static const int unit_maxhp[] = 	{ 0, 5000000, 100000, 100000, 500000, 100000, 100000 };
static const int unit_maxshield[] = 	{ 0, 5000000, 1, 500000, 500000, 4000000, 100000 };		/* Shield must not be 0 for any existing building! */
static const int unit_shieldreg[] = 	{ 0, 5, 5, 5, 5, 5, 5 };
/* Pylons and generators must have the same distribution range... */
static const int unit_range[] = 	{ 0, 4, 0, 3, 4, 1, 1 };
static const int unit_buildtime[] =	{ 0, 0, 5000, 20000, 20000, 10000, 10000 };

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
void unitAnnounce(int from, int to, int building, int index);
int unitSpawn(unsigned int player, unsigned int unit, unsigned int x, unsigned int y);
int unitAdd(int owner, int type, int x, int y);
int unitRemove(int x, int y);
void unitDestroy(int player, unsigned int index);
void unitPylonPulse();

void unitLoop(int msec);


#endif
