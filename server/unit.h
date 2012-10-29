#ifndef __UNIT_H__
#define	__UNIT_H__

#define	UNIT_NOTHING			0
#define	UNIT_GENERATOR			1
#define	UNIT_SCOUT			2
#define	UNIT_ATTACKER			3
#define	UNIT_PYLON			4
#define	UNIT_WALL			5


typedef struct {
	unsigned int		max_hp;
	unsigned int		max_shield;
} UNIT_DEF;

int unitLOS(int unit);
 int unitAdd(int owner, int type, int x, int y) ;


#endif
