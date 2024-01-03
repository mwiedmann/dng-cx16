#ifndef AI_H
#define AI_H

#include "globals.h"

// How many 4 tick chunks to lead player on a lob
// The tick count would be LOB_PLAYER_CHUNK*5
#define LOB_PLAYER_CHUNK 15

// This is ((LOB_PLAYER_CHUNK * 4) / 2)
// Entities move 30/sec
#define LOB_MOVE_TICKS 30

#define LOB_PIXEL_MAX 16*5

#define RANGED_SPEED 4

void toggleEntity(unsigned char spriteId, unsigned char show);
void activateEntities(short scrollX, short scrollY);
void deactivateEntities(short scrollX, short scrollY);
void tempActiveToActiveEntities();

void moveEntity(Entity *entity, short scrollX, short scrollY);
void attackEntity(unsigned char playerId, Entity *entity, unsigned char damage);
void useScrollOnEntities(unsigned char playerId);
#endif