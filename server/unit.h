#ifndef __UNIT_H__
#define	__UNIT_H__

#define	UNITS_DEFINED			5

#define	UNIT_DEF_NOTHING		0
#define	UNIT_DEF_GENERATOR		1
#define	UNIT_DEF_SCOUT			2
#define	UNIT_DEF_ATTACKER		3
#define	UNIT_DEF_PYLON			4
#define	UNIT_DEF_WALL			5

#define	UNIT_ATTACKER_DMGP		5

static const int unit_los[] = 		{ 0, 7, 6, 3, 4, 1 };
static const int unit_maxhp[] = 	{ 0, 50000, 1000, 1000, 5000, 1000 };
static const int unit_maxshield[] = 	{ 0, 50000, 0, 5000, 5000, 40000 };
static const int unit_shieldreg[] = 	{ 0, 40, 1, 10, 10, 5 };
static const int unit_range[] = 	{ 0, 4, 0, 3, 4, 1 };
static const int unit_buildtime[] =	{ 0, 0, 5000, 20000, 20000, 10000 };

struct SERVER_UNIT;


struct UNIT_PYLON;

typedef struct UNIT_PYLON_LIST {
	struct UNIT_PYLON		*pylon;
	struct UNIT_PYLON_LIST		*next;
} UNIT_PYLON_LIST;

typedef struct UNIT_PYLON {
	unsigned int			power	: 1;
	unsigned int			pulse	: 1;
	unsigned int			x;
	unsigned int			y;
	struct SERVER_UNIT		*unit;
	struct UNIT_PYLON_LIST		*next;
} UNIT_PYLON;


typedef struct {
	unsigned int		max_hp;
	unsigned int		max_shield;
} UNIT_DEF;

int unitLOS(int unit);
int unitMaxHP(int unit);
int unitRange(int type);
int unitSpawn(unsigned int player, unsigned int unit, unsigned int x, unsigned int y);
int unitAdd(int owner, int type, int x, int y);
int unitRemove(int x, int y);



#endif
