#include <cx16.h>

#include "map.h"
#include "globals.h"

// Point this array to hi-ram
unsigned char (*mapStatus)[MAP_MAX] = (unsigned char (*)[MAP_MAX])BANK_RAM;

Entity entityList[ENTITY_COUNT];

Entity *entitySleepList = 0;
Entity *entityActiveList = 0;

Entity *entityTempActiveList = 0;

Guy guy;
