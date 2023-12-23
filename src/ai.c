#include <stdlib.h>
#include <stdio.h>

#include "map.h"
#include "sprites.h"
#include "globals.h"
#include "ai.h"
#include "tiles.h"
#include "list.h"

void toggleEntity(unsigned char spriteId, unsigned char show) {
    unsigned long spriteAddr = SPRITE1_ADDR + (spriteId * 8);

    toggleSprite(spriteAddr, show);
}

void activateEntities(unsigned char guyTileX, unsigned char guyTileY) {
    Entity *entity;
    Entity *nextEntity;

    entity = entitySleepList;
    entityTempActiveList = 0;
    
    // Make sure we have at least 1 sleeping entity or there is nothing to do
    if (!entity) {
        return;
    }

    do {
        nextEntity = entity->next;

        // Activate this entity if it is within range
        if (abs(guyTileX - entity->currentTileX) <= DIST_MAX_X && abs(guyTileY - entity->currentTileY) <= DIST_MAX_Y) {
            toggleEntity(entity->spriteId, 1);
            entity->visible = 1;
            // printf("Moving entity to tempActive from sleep: %i\n", entity->spriteId);
            // moveEntityFromSleepToTempActiveList(entity);
            moveEntityToList(entity, &entityTempActiveList, &entitySleepList);
        } 
        // else {
        //     // printf("Out of range: %i x:%i y:%i\n", entity->spriteId, entity->currentTileX, entity->currentTileY);
        // }

        entity = nextEntity;
    } while (entity);
}

void deactivateEntities(unsigned char guyTileX, unsigned char guyTileY) {
    Entity *entity;
    Entity *nextEntity;

    entity = entityActiveList;
    
    // Make sure we have at least 1 active entity or there is nothing to do
    if (!entity) {
        // printf("No active entities to deactivate\n");
        return;
    }

    do {
        nextEntity = entity->next;

        // Deactivate this entity if it is out of range
        if (abs(guyTileX - entity->currentTileX) > DIST_MAX_X || abs(guyTileY - entity->currentTileY) > DIST_MAX_Y) {
            toggleEntity(entity->spriteId, 0);
            entity->visible = 0;
            // printf("Moving entity to sleep from active: %i\n", entity->spriteId);
            moveEntityToList(entity, &entitySleepList, &entityActiveList);
        }

        entity = nextEntity;
    } while (entity);
}

void tempActiveToActiveEntities() {
    Entity *entity;
    Entity *nextEntity;

    entity = entityTempActiveList;
    
    // Now for the final step, move the entityTempActiveList into the activeList

    // Make sure we have at least 1 active entity or there is nothing to do
    if (!entity) {
        // printf("No tempActive entities to copy to active\n");
        return;
    }

    do {
        nextEntity = entity->next;
        
        // printf("Moving entity to active from tempActive: %i\n", entity->spriteId);
        moveEntityToList(entity, &entityActiveList, &entityTempActiveList);

        entity = nextEntity;
    } while (entity);

    entityTempActiveList = 0;
}

void moveEntity(Entity *entity, unsigned char guyTileX, unsigned char guyTileY, short scrollX, short scrollY) {
    unsigned char newTileX, newTileY;
    signed char tileXChange = 0, tileYChange = 0;
    unsigned char distX, distY;

    if (!entity->hasTarget) {
        // Try to move X towards the guy
        if (entity->currentTileX < guyTileX) {
            tileXChange = 1;
        } else if (entity->currentTileX > guyTileX) {
            tileXChange = -1;
        }

        // Try to move Y towards guy
        if (entity->currentTileY < guyTileY) {
            tileYChange = 1;
        } else if (entity->currentTileY > guyTileY) {
            tileYChange = -1;
        }

        // If X direction is blocked, don't move that axis
        if (tileXChange != 0 && mapStatus[entity->currentTileY][entity->currentTileX+tileXChange] > TILE_FLOOR) {
            tileXChange = 0;
        }

        // If Y direction is blocked, don't move that axis
        if (tileYChange != 0 && mapStatus[entity->currentTileY+tileYChange][entity->currentTileX] > TILE_FLOOR) {
            tileYChange = 0;
        }

        // If diagonal, check for blocking tiles
        if (tileXChange !=0 && tileYChange != 0 && mapStatus[entity->currentTileY+tileYChange][entity->currentTileX+tileXChange] > TILE_FLOOR) {
            distX = abs(guyTileX - entity->currentTileX);
            distY = abs(guyTileY - entity->currentTileY);

            // Move whichever direction is farther to close the gap
            if (distX > distY) {
                tileYChange = 0;
            } else {
                tileXChange = 0;
            }
        }

        if (tileXChange !=0 || tileYChange !=0) {
            // Set the new start and target for the entity
            entity->startTileX = entity->currentTileX;
            entity->startTileY = entity->currentTileY;
            entity->targetTileX = entity->currentTileX+tileXChange;
            entity->targetTileY = entity->currentTileY+tileYChange;
    
            // Claim the tile and now entity has a target
            mapStatus[entity->targetTileY][entity->targetTileX] = ENTITY_CLAIM;
            entity->hasTarget = 1;
        }
    } else {
        // Try to move X towards the target
        if (entity->currentTileX < entity->targetTileX) {
            tileXChange = 1;
        } else if (entity->currentTileX > entity->targetTileX) {
            tileXChange = -1;
        }

        // Try to move Y towards target
        if (entity->currentTileY < entity->targetTileY) {
            tileYChange = 1;
        } else if (entity->currentTileY > entity->targetTileY) {
            tileYChange = -1;
        }
    }

    if (tileXChange != 0) {
        entity->x+= tileXChange * AI_SPEED;
    } else {
        if (entity->x+8 < ((entity->currentTileX)<<4)+8) {
            entity->x+= AI_SPEED;
        } else if (entity->x+8 > ((entity->currentTileX)<<4)+8) {
            entity->x-= AI_SPEED;
        }
    }

    if (tileYChange != 0) {
        entity->y+= tileYChange * AI_SPEED;
    } else {
        if (entity->y+8 < ((entity->currentTileY)<<4)+8) {
            entity->y+= AI_SPEED;
        } else if (entity->y+8 > ((entity->currentTileY)<<4)+8) {
            entity->y-= AI_SPEED;
        }
    }

    // If moved towards another tile...
    if (tileXChange != 0 || tileYChange != 0) {
        // Get the new tile
        newTileX = (entity->x + 8) >> 4;
        newTileY = (entity->y + 8) >> 4;

        // Update the current tile
        entity->currentTileX = newTileX;
        entity->currentTileY = newTileY;

        // See if the entity has moved to its target tile
        if (entity->targetTileX == newTileX && entity->targetTileY == newTileY) {
            // Clear the old tile and mark the new tile as blocked
            mapStatus[entity->startTileY][entity->startTileX] = 0; // Remove target blocker (can be diff) from actual new tile
            mapStatus[entity->targetTileY][entity->targetTileX] = ENTITY_TILE_START + entity->spriteId; // Block new tile
            entity->hasTarget = 0; // Will need new target
        }
    }

    // Show the sprite if currently not visible
    if (!entity->visible) {
        toggleEntity(entity->spriteId, 1);
        entity->visible = 1;
    }

    moveSpriteId(entity->spriteId, entity->x, entity->y, scrollX, scrollY);
}
