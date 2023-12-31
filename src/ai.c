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

void activateEntities(short scrollX, short scrollY) {
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

        // Activate this entity if it is on the screen
        if (entity->x >= scrollX + SCROLL_PIXEL_SIZE || entity->x+16 <= scrollX || entity->y >= scrollY + SCROLL_PIXEL_SIZE || entity->y+16 <= scrollY) {
            // Off screen
        } else {
            entity->visible = 1;
            moveAndSetAnimationFrame(entity->spriteId, entity->x, entity->y, scrollX, scrollY, entity->tileId, entity->animationFrame, entity->facingX);
            moveEntityToList(entity, &entityTempActiveList, &entitySleepList);
        }

        entity = nextEntity;
    } while (entity);
}

void deactivateEntities(short scrollX, short scrollY) {
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
        if (entity->x >= scrollX + SCROLL_PIXEL_SIZE || entity->x+16 <= scrollX || entity->y >= scrollY + SCROLL_PIXEL_SIZE || entity->y+16 <= scrollY) {
            // Off screen
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

void attackEntity(unsigned char playerId, Entity *entity, unsigned char damage) {
    if (entity->health > damage) {
        entity->health -= damage;
    } else {
        players[playerId].score += entity->points;
        entity->health = 0;
        mapStatus[entity->currentTileY][entity->currentTileX] = TILE_FLOOR;
        if (entity->hasTarget) {
            mapStatus[entity->targetTileY][entity->targetTileX] = TILE_FLOOR;
        }
        toggleEntity(entity->spriteId, 0);
        deleteEntityFromList(entity, &entityActiveList);
    }
}

void meleeAttackGuy(unsigned char playerId, unsigned char dmg) {
    if (players[playerId].health > dmg) {
        players[playerId].health -= dmg;
    } else {
        players[playerId].health = 0;
        players[playerId].active = 0;
    }
}

void moveEntity(Entity *entity, unsigned char guyTileX, unsigned char guyTileY, short scrollX, short scrollY) {
    unsigned char newTileX, newTileY, i;
    signed char tileXChange = 0, tileYChange = 0, x, y;
    unsigned char distX, distY;
    unsigned short prevX, prevY;
    unsigned char attack = 0, needsMove=1, foundEmptyTile=0;

    if (entity->isGenerator) {
        if (entity->nextSpawn > 0) {
            entity->nextSpawn -= 1;
        } else {
            entity->nextSpawn = entity->spawnRate;
            // Create an entity
            // Find an empty tile
            if (guyTileX < entity->currentTileX) {
                newTileX = entity->currentTileX-1;
            } else if (guyTileX > entity->currentTileX) {
                newTileX = entity->currentTileX+1;
            } else {
                newTileX = entity->currentTileX;
            }

            if (guyTileY < entity->currentTileY) {
                newTileY = entity->currentTileY-1;
            } else if (guyTileY > entity->currentTileY) {
                newTileY = entity->currentTileY+1;
            } else {
                newTileY = entity->currentTileY;
            }
            
            if (mapStatus[newTileY][newTileX] != TILE_FLOOR) {
                for (y=-1; y <= 1 && !foundEmptyTile; y++) {
                    for (x=-1; x <= 1; x++) {
                        if (mapStatus[entity->currentTileY+y][entity->currentTileX+x] == TILE_FLOOR) {
                            newTileY = entity->currentTileY+y;
                            newTileX = entity->currentTileX+x;
                            foundEmptyTile = 1;
                            break;
                        }
                    }
                }
            } else {
                foundEmptyTile = 1;
            }

            // If we have an empty tile, find unused entity and spawn it
            if (foundEmptyTile) {
                for (i=0; i < ENTITY_COUNT; i++) {
                    if (entityList[i].health == 0) {
                        createEntity(TILE_ENTITY_START+entity->entityTypeId, i, newTileX, newTileY);
                        addNewEntityToList(&entityList[i], &entitySleepList);
                        mapStatus[newTileY][newTileX] = ENTITY_TILE_START + entityList[i].spriteId;
                        break;
                    }
                }
            }
        }

        if (entity->animationChange) {
            entity->animationChange = 0;
            moveAndSetAnimationFrame(entity->spriteId, entity->x, entity->y, scrollX, scrollY, entity->tileId, 0, 0);
        } else {
            moveSpriteId(entity->spriteId, entity->x, entity->y, scrollX, scrollY);
        }

        return;
    }

    prevX = entity->x;
    prevY = entity->y;

    entity->statsId += 1;
    if (entity->statsId == 4) {
        entity->statsId = 0;
    }

    if (!entity->hasTarget) {
        // Try to move X towards the guy
        if (entity->currentTileX < guyTileX) {
            tileXChange = 1;
            entity->facingX = 0;
        } else if (entity->currentTileX > guyTileX) {
            tileXChange = -1;
            entity->facingX = 1;
        }

        // Try to move Y towards guy
        if (entity->currentTileY < guyTileY) {
            tileYChange = 1;
        } else if (entity->currentTileY > guyTileY) {
            tileYChange = -1;
        }

        // If X direction is blocked, don't move that axis
        if (tileXChange != 0 && mapStatus[entity->currentTileY][entity->currentTileX+tileXChange] > TILE_FLOOR) {
            if (mapStatus[entity->currentTileY][entity->currentTileX+tileXChange] >= GUY_CLAIM) {
                attack = mapStatus[entity->currentTileY][entity->currentTileX+tileXChange];
            } else {
                tileXChange = 0;
            }
        }

        // If Y direction is blocked, don't move that axis
        if (tileYChange != 0 && mapStatus[entity->currentTileY+tileYChange][entity->currentTileX] > TILE_FLOOR) {
            if (mapStatus[entity->currentTileY+tileYChange][entity->currentTileX] >= GUY_CLAIM) {
                attack = mapStatus[entity->currentTileY+tileYChange][entity->currentTileX];
            } else {
                tileYChange = 0;
            }
        }

        // If diagonal, check for blocking tiles
        if (tileXChange !=0 && tileYChange != 0 && mapStatus[entity->currentTileY+tileYChange][entity->currentTileX+tileXChange] > TILE_FLOOR) {
            if (mapStatus[entity->currentTileY+tileYChange][entity->currentTileX+tileXChange] >= GUY_CLAIM) {
                attack = mapStatus[entity->currentTileY+tileYChange][entity->currentTileX+tileXChange];
            } else {
                distX = abs(guyTileX - entity->currentTileX);
                distY = abs(guyTileY - entity->currentTileY);

                // Move whichever direction is farther to close the gap
                if (distX > distY) {
                    tileYChange = 0;
                } else {
                    tileXChange = 0;
                }
            }
        }

        if (attack>0) {
            // Keep these until now so we can face the target
            tileXChange = 0;
            tileYChange = 0;

            meleeAttackGuy(GUY_CLAIM-attack, entity->stats->melee[entity->statsId]);
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
            entity->facingX = 0;
        } else if (entity->currentTileX > entity->targetTileX) {
            tileXChange = -1;
            entity->facingX = 1;
        }

        // Try to move Y towards target
        if (entity->currentTileY < entity->targetTileY) {
            tileYChange = 1;
        } else if (entity->currentTileY > entity->targetTileY) {
            tileYChange = -1;
        }
    }

    if (tileXChange != 0) {
        entity->x+= tileXChange * entity->stats->moves[entity->statsId];
    } else {
        if (entity->x+8 < ((entity->currentTileX)<<4)+8) {
            entity->x+= entity->stats->moves[entity->statsId];
        } else if (entity->x+8 > ((entity->currentTileX)<<4)+8) {
            entity->x-= entity->stats->moves[entity->statsId];
        }
    }

    if (tileYChange != 0) {
        entity->y+= tileYChange * entity->stats->moves[entity->statsId];
    } else {
        if (entity->y+8 < ((entity->currentTileY)<<4)+8) {
            entity->y+= entity->stats->moves[entity->statsId];
        } else if (entity->y+8 > ((entity->currentTileY)<<4)+8) {
            entity->y-= entity->stats->moves[entity->statsId];
        }
    }

    // If moved towards another tile...
    if (tileXChange != 0 || tileYChange != 0) {
        // Get the new tile
        newTileX = (entity->x + 8) >> 4;
        newTileY = (entity->y + 8) >> 4;

        // See if the entity has moved to its target tile
        if (entity->targetTileX == newTileX && entity->targetTileY == newTileY) {
            // See if guy is in this tile...if so, stay still, but attack!
            if (mapStatus[newTileY][newTileX] >= GUY_CLAIM) {
                entity->x = prevX;
                entity->y = prevY;

                meleeAttackGuy(GUY_CLAIM-mapStatus[newTileY][newTileX], entity->stats->melee[entity->statsId]);
            } else {
                // Clear the old tile and mark the new tile as blocked
                mapStatus[entity->startTileY][entity->startTileX] = TILE_FLOOR; // Remove target blocker (can be diff) from actual new tile
                mapStatus[entity->targetTileY][entity->targetTileX] = ENTITY_TILE_START + entity->spriteId; // Block new tile
                entity->hasTarget = 0; // Will need new target
                
                // Update the current tile
                entity->currentTileX = newTileX;
                entity->currentTileY = newTileY;
            }
        }
    }

    if (entity->x != prevX || entity->y != prevY) {
        if (entity->animationCount == 0) {
            entity->animationCount = ANIMATION_FRAME_SPEED;
            if (entity->animationFrame == ANIMATION_FRAME_COUNT-1) {
                entity->animationFrame = 0;
            } else {
                entity->animationFrame += 1;
            }

            moveAndSetAnimationFrame(entity->spriteId, entity->x, entity->y, scrollX, scrollY, entity->tileId, entity->animationFrame, entity->facingX);
            needsMove = 0;
        } else {
            entity->animationCount -= 1;
        }
    }

    if (needsMove) {
        moveSpriteId(entity->spriteId, entity->x, entity->y, scrollX, scrollY);
    }
}
