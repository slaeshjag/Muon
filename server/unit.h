#ifndef __UNIT_H__
#define	__UNIT_H__

#define	UNITS_DEFINED			5

#define	UNIT_NOTHING			0
#define	UNIT_GENERATOR			1
#define	UNIT_SCOUT			2
#define	UNIT_ATTACKER			3
#define	UNIT_PYLON			4
#define	UNIT_WALL			5

static const int unit_los[] = 		{ 0, 7, 6, 3, 4, 1 };
static const int unit_maxhp[] = 	{ 0, 5000, 100, 1000, 500, 4000 };
static const int unit_maxshield[] = 	{ 0, 5000, 50, 500, 500, 4000 };
static const int unit_shieldrec[] = 	{ 0, 40, 1, 10, 10, 5 };
static const int unit_range[] = 	{ 0, 4, 0, 3, 4, 1 };


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
