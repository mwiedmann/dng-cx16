#ifndef GLOBALS_H
#define GLOBALS_H

#include "map.h"

#define ENTITY_COUNT 75
#define GUY_SPEED_1 1
#define GUY_SPEED_2 2
#define WEAPON_SPEED 4
#define WEAPON_ROTATION_SPEED 4
#define GUY_SHOOT_TICKS 10
#define WEAPON_DAMAGE 30
#define MELEE_DAMAGE 10

#define NUM_PLAYERS 2

extern unsigned char (*mapStatus)[MAP_MAX];

extern Entity entityList[ENTITY_COUNT];
extern Entity *entitySleepList;
extern Entity *entityActiveList;
extern Entity *entityTempActiveList;

extern Guy players[NUM_PLAYERS];
extern Weapon weapons[NUM_PLAYERS];

extern unsigned short maxMapX;
extern unsigned short maxMapY;

#endif