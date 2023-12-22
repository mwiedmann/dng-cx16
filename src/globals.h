#ifndef GLOBALS_H
#define GLOBALS_H

#include "map.h"

#define ENTITY_COUNT 50
#define GUY_SPEED 2

extern unsigned char (*mapStatus)[MAP_MAX];

extern Entity entityList[ENTITY_COUNT];
extern Entity *entitySleepList;
extern Entity *entityActiveList;
extern Entity *entityTempActiveList;

extern Guy guy;

#endif