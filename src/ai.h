#ifndef AI_H
#define AI_H

#include "globals.h"

#define AI_SPEED 2
#define DIST_MAX_X 10  // In tiles
#define DIST_MAX_Y 8

void toggleEntity(unsigned char spriteId, unsigned char show);
void activateEntities(unsigned char guyTileX, unsigned char guyTileY);
void deactivateEntities(unsigned char guyTileX, unsigned char guyTileY);
void tempActiveToActiveEntities();

void moveEntity(Entity *entity, unsigned char guyTileX, unsigned char guyTileY, short scrollX, short scrollY);

#endif