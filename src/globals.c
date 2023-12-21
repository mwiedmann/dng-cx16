#include "map.h"
#include "globals.h"

unsigned char mapStatus[MAP_MAX][MAP_MAX];

Entity entityList[ENTITY_COUNT];

Entity *entitySleepList = 0;
Entity *entityActiveList = 0;

Entity *entityTempActiveList = 0;

Guy guy = { 320, 240 };
