#include <stdlib.h>

#include "map.h"
#include "sprites.h"
#include "globals.h"
#include "ai.h"
#include "tiles.h"

extern unsigned char mapStatus[MAP_MAX][MAP_MAX];

void toggleEntity(unsigned char entityId, unsigned char show) {
    unsigned long spriteAddr = SPRITE1_ADDR + ((entityId+1) * 8);

    toggleSprite(spriteAddr, show);
}

void moveEntity(unsigned char entityId, unsigned char guyTileX, unsigned char guyTileY, short scrollX, short scrollY) {
    unsigned char newTileX, newTileY;
    signed char tileXChange = 0, tileYChange = 0;
    unsigned char distX, distY;

    distX = abs(guyTileX - entityList[entityId].currentTileX);
    // Don't activate this entity if its too far away from the player
    if (distX > DIST_MAX_X) {
        // Hide the sprite if currently visible
        if (entityList[entityId].visible) {
            toggleEntity(entityId, 0);
            entityList[entityId].visible = 0;
        }
        return;
    }

    distY = abs(guyTileY - entityList[entityId].currentTileY);
    // Don't activate this entity if its too far away from the player
    if (distY > DIST_MAX_Y) {
        // Hide the sprite if currently visible
        if (entityList[entityId].visible) {
            toggleEntity(entityId, 0);
            entityList[entityId].visible = 0;
        }
        return;
    }

    if (!entityList[entityId].hasTarget) {
        // Try to move X towards the guy
        if (entityList[entityId].currentTileX < guyTileX) {
            tileXChange = 1;
        } else if (entityList[entityId].currentTileX > guyTileX) {
            tileXChange = -1;
        }

        // Try to move Y towards guy
        if (entityList[entityId].currentTileY < guyTileY) {
            tileYChange = 1;
        } else if (entityList[entityId].currentTileY > guyTileY) {
            tileYChange = -1;
        }

        // If X direction is blocked, don't move that axis
        if (tileXChange != 0 && mapStatus[entityList[entityId].currentTileY][entityList[entityId].currentTileX+tileXChange] != 0) {
            tileXChange = 0;
        }

        // If Y direction is blocked, don't move that axis
        if (tileYChange != 0 && mapStatus[entityList[entityId].currentTileY+tileYChange][entityList[entityId].currentTileX] != 0) {
            tileYChange = 0;
        }

        // If diagonal, check for blocking tiles
        if (tileXChange !=0 && tileYChange != 0 && mapStatus[entityList[entityId].currentTileY+tileYChange][entityList[entityId].currentTileX+tileXChange] != 0) {
            // Move whichever direction is farther to close the gap
            if (distX > distY) {
                tileYChange = 0;
            } else {
                tileXChange = 0;
            }
        }

        if (tileXChange !=0 || tileYChange !=0) {
            // Set the new start and target for the entity
            entityList[entityId].startTileX = entityList[entityId].currentTileX;
            entityList[entityId].startTileY = entityList[entityId].currentTileY;
            entityList[entityId].targetTileX = entityList[entityId].currentTileX+tileXChange;
            entityList[entityId].targetTileY = entityList[entityId].currentTileY+tileYChange;
    
            // Claim the tile and now entity has a target
            mapStatus[entityList[entityId].targetTileY][entityList[entityId].targetTileX] = ENTITY_CLAIM;
            entityList[entityId].hasTarget = 1;
        }
    } else {
        // Try to move X towards the target
        if (entityList[entityId].currentTileX < entityList[entityId].targetTileX) {
            tileXChange = 1;
        } else if (entityList[entityId].currentTileX > entityList[entityId].targetTileX) {
            tileXChange = -1;
        }

        // Try to move Y towards target
        if (entityList[entityId].currentTileY < entityList[entityId].targetTileY) {
            tileYChange = 1;
        } else if (entityList[entityId].currentTileY > entityList[entityId].targetTileY) {
            tileYChange = -1;
        }
    }

    if (tileXChange != 0) {
        entityList[entityId].x+= tileXChange * AI_SPEED;
    } else {
        if (entityList[entityId].x+8 < ((entityList[entityId].currentTileX)<<4)+8) {
            entityList[entityId].x+= AI_SPEED;
        } else if (entityList[entityId].x+8 > ((entityList[entityId].currentTileX)<<4)+8) {
            entityList[entityId].x-= AI_SPEED;
        }
    }

    if (tileYChange != 0) {
        entityList[entityId].y+= tileYChange * AI_SPEED;
    } else {
        if (entityList[entityId].y+8 < ((entityList[entityId].currentTileY)<<4)+8) {
            entityList[entityId].y+= AI_SPEED;
        } else if (entityList[entityId].y+8 > ((entityList[entityId].currentTileY)<<4)+8) {
            entityList[entityId].y-= AI_SPEED;
        }
    }

    // If moved towards another tile...
    if (tileXChange != 0 || tileYChange != 0) {
        // Get the new tile
        newTileX = (entityList[entityId].x + 8) >> 4;
        newTileY = (entityList[entityId].y + 8) >> 4;

        // Update the current tile
        entityList[entityId].currentTileX = newTileX;
        entityList[entityId].currentTileY = newTileY;

        // See if the entity has moved to its target tile
        if (entityList[entityId].targetTileX == newTileX && entityList[entityId].targetTileY == newTileY) {
            // Clear the old tile and mark the new tile as blocked
            mapStatus[entityList[entityId].startTileY][entityList[entityId].startTileX] = 0; // Remove target blocker (can be diff) from actual new tile
            mapStatus[entityList[entityId].targetTileY][entityList[entityId].targetTileX] = ENTITY_TILE_START + entityId; // Block new tile
            entityList[entityId].hasTarget = 0; // Will need new target
        }
    }

    // Show the sprite if currently not visible
    if (!entityList[entityId].visible) {
        toggleEntity(entityId, 1);
        entityList[entityId].visible = 1;
    }

    moveSpriteId(entityId+1, entityList[entityId].x, entityList[entityId].y, scrollX, scrollY);
}
