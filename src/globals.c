#include <cx16.h>

#include "map.h"
#include "globals.h"

// Point this array to hi-ram
unsigned char (*mapStatus)[MAP_MAX] = (unsigned char (*)[MAP_MAX])BANK_RAM;

Entity entityList[ENTITY_COUNT];

Entity *entitySleepList = 0;
Entity *entityActiveList = 0;

Entity *entityTempActiveList = 0;

Guy players[NUM_PLAYERS];
Weapon weapons[NUM_PLAYERS];

unsigned short maxMapX;
unsigned short maxMapY;

unsigned char entityWeakMelee[2] = { 1, 1 };
unsigned char entityNormalMelee[2] = { 2, 1 };
unsigned char entityStrongMelee[2] = { 2, 2 };

unsigned char entitySlowMove[2] = { 1, 1 };
unsigned char entityMediumMove[2] = { 2, 1 };
unsigned char entityFastMove[2] = { 2, 2 };


EntityStats skeletonStats = {
    entityWeakMelee,
    entityMediumMove,
    0, // ranged
};