#ifndef AI_H
#define AI_H

#include "globals.h"

void toggleEntity(unsigned char spriteId, unsigned char show);
void activateEntities(short scrollX, short scrollY);
void deactivateEntities(short scrollX, short scrollY);
void tempActiveToActiveEntities();

void moveEntity(Entity *entity, unsigned char guyTileX, unsigned char guyTileY, short scrollX, short scrollY);
void attackEntity(unsigned char playerId, Entity *entity, unsigned char damage);
#endif