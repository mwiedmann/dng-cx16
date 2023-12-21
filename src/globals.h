#ifndef GLOBALS_H
#define GLOBALS_H

#include "map.h"

#define ENTITY_COUNT 48
#define GUY_SPEED 2

extern unsigned char mapStatus[MAP_MAX][MAP_MAX];

extern Entity entityList[ENTITY_COUNT];
extern Guy guy;

#endif