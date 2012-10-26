#ifndef __MAP_PLATFORM_H__
#define	__MAP_PLATFORM_H__


void *ldmzLoad(const char *fname);
void *ldmzFree(void *map);
void ldmzGetSize(void *map, int *x, int *y);
unsigned int *ldmzGetData(void *map);

#endif
