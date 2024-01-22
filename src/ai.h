#ifndef AI_H
#define AI_H

#include "globals.h"

// How many 6 tick chunks to lead player on a lob
// The tick count would be LOB_PLAYER_CHUNK*4
#define LOB_PLAYER_CHUNK 10

// This is ((LOB_PLAYER_CHUNK * 6) / 2)
// Entities move 30/sec
#define LOB_MOVE_TICKS 30

#define LOB_PIXEL_MAX 16*5

#define RANGED_SPEED 4

void activateEntities();
void deactivateEntities();
void tempActiveToActiveEntities();

void meleeAttackGuy(unsigned char playerId, unsigned char statsId, unsigned char dmg, unsigned char showDmg);
void moveEntity(Entity *entity);
void attackEntity(unsigned char playerId, Entity *entity, unsigned char damage);
void useScrollOnEntities(unsigned char playerId);
#endif