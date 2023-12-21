#ifndef AI_H
#define AI_H

#define AI_SKIP 2
#define AI_SPEED AI_SKIP
#define DIST_MAX_X 10  // In tiles
#define DIST_MAX_Y 8

void moveEntity(unsigned char entityId, unsigned char guyTileX, unsigned char guyTileY, short scrollX, short scrollY);

#endif