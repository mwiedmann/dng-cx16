#include "map.h"
#include "globals.h"

unsigned char mapStatus[MAP_MAX][MAP_MAX];

Entity entityList[ENTITY_COUNT];
Guy guy = { 320, 240 };