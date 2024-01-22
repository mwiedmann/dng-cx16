#include <stdlib.h>
#include <stdio.h>
#include <cx16.h>

#include "map.h"
#include "sprites.h"
#include "globals.h"
#include "ai.h"
#include "tiles.h"
#include "list.h"
#include "players.h"
#include "config.h"
#include "utils.h"
#include "sound.h"

void activateEntities() {
    Entity *entity;
    Entity *nextEntity;

    // Start the active list at 0
    activeEntityCount = 0;
    totalEntityCount = 0;

    entity = entitySleepList;
    entityTempActiveList = 0;
    
    // Make sure we have at least 1 sleeping entity or there is nothing to do
    if (!entity) {
        return;
    }

    do {
        totalEntityCount += 1;
        nextEntity = entity->next;

        // Activate this entity if it is on the screen
        if (entity->x >= scrollX + SCROLL_PIXEL_SIZE || entity->x+16 <= scrollX || entity->y >= scrollY + SCROLL_PIXEL_SIZE || entity->y+16 <= scrollY) {
            // Off screen
        } else {
            entity->visible = 1;
            moveAndSetAnimationFrame(entity->spriteAddrLo, entity->spriteAddrHi, entity->spriteGraphicLo, entity->spriteGraphicHi,
                entity->x, entity->y, entity->animationFrame, entity->facingX, 0);
            moveEntityToList(entity, &entityTempActiveList, &entitySleepList);
        }

        entity = nextEntity;
    } while (entity);
}

void deactivateEntities() {
    Entity *entity;
    Entity *nextEntity;

    entity = entityActiveList;
    
    // Make sure we have at least 1 active entity or there is nothing to do
    if (!entity) {
        // printf("No active entities to deactivate\n");
        return;
    }

    do {
        totalEntityCount+= 1;
        nextEntity = entity->next;

        // Deactivate this entity if it is out of range
        if (entity->x >= scrollX + SCROLL_PIXEL_SIZE || entity->x+16 <= scrollX || entity->y >= scrollY + SCROLL_PIXEL_SIZE || entity->y+16 <= scrollY) {
            // Off screen
            toggleSprite(entity->spriteAddrLo, entity->spriteAddrHi, 0);
            entity->visible = 0;
            moveEntityToList(entity, &entitySleepList, &entityActiveList);
        } else {
            activeEntityCount += 1;
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
        
        moveEntityToList(entity, &entityActiveList, &entityTempActiveList);

        activeEntityCount += 1;

        entity = nextEntity;
    } while (entity);

    entityTempActiveList = 0;
}

void attackEntity(unsigned char playerId, Entity *entity, unsigned char damage) {
    if (entity->health > damage) {
        entity->health -= damage;
    } else {
        overlayChanged = 1;
        players[playerId].score += entity->points;
        entity->health = 0;
        
        // Took this out because the gold left behind would block player shots.
        // This was a punishment if there were other generators/entities behind it.
        // Cool idea but think more about this.
        // // Generators leave treasure behind
        // if (entity->isGenerator) {
        //     mapStatus[entity->currentTileY][entity->currentTileX] = TILE_TREASURE_SILVER;
        //     l0TileShow(entity->currentTileX, entity->currentTileY, 46);
        // } else {
        
        mapStatus[entity->currentTileY][entity->currentTileX] = TILE_FLOOR;

        if (entity->hasTarget) {
            mapStatus[entity->targetTileY][entity->targetTileX] = TILE_FLOOR;
        }
        toggleSprite(entity->spriteAddrLo, entity->spriteAddrHi, 0);
        deleteEntityFromList(entity, &entityActiveList);
    }
}

void meleeAttackGuy(unsigned char playerId, unsigned char statsId, unsigned char dmg, unsigned char showDmg) {
    unsigned char genEntityId;
    signed char adjustedDmg = dmg - players[playerId].stats->armor[statsId];

    // Make sure the player is still active
    if (!players[playerId].active) {
        return;
    }

    // See if all damage blocked
    if (adjustedDmg<=0) {
        return;
    }

    overlayChanged = 1;

    // Show the player being hit for a few frames
    if (showDmg && !players[playerId].wasHit) {
        players[playerId].wasHit = PLAYER_HIT_ANIM_FRAMES;
    }

    if (players[playerId].health > adjustedDmg) {
        players[playerId].health -= adjustedDmg;
    } else {
        genEntityId = destroyPlayer(playerId);
        // Update the map here because we switched banks
        mapStatus[players[playerId].currentTileY][players[playerId].currentTileX] = ENTITY_TILE_START + entityList[genEntityId].spriteId;
        
    }
}

Guy *getClosestPlayer(unsigned short x, unsigned short y) {
    unsigned char i, closest;
    unsigned short amount=32767, tempA, tempB;

    // If only 1 player active, return it
    if (!players[1].active) {
        return &players[0];
    } else if (!players[0].active) {
        return &players[1];
    }

    for (i=0, closest=0; i<NUM_PLAYERS; i++) {
        tempA = abs(x - players[i].x);
        tempB = abs(y - players[i].y);

        // Largest will be the distance of the entity to the x/y
        // This is because entities can move diagonally
        // Use tempA to hold the largest
        if (tempB > tempA) {
            tempA = tempB;
        }

        // If this player is closer, make it the target
        if (tempA < amount) {
            closest = i;
            amount = tempA;
        }
    }

    return &players[closest];
}

void moveEntity(Entity *entity) {
    unsigned char newTileX, newTileY, i;
    signed char tileXChange = 0, tileYChange = 0, x, y;
    unsigned char distX, distY;
    unsigned short prevX, prevY;
    unsigned char attack = 0, needsMove=1, foundEmptyTile=0;
    unsigned char guyTileX, guyTileY;
    signed short xTemp, yTemp;
    Guy *guy;

    guy = getClosestPlayer(entity->x, entity->y);

    guyTileX = guy->currentTileX;
    guyTileY = guy->currentTileY;

    // Is this a projectile entity?
    if (entity->isShot) {
        entity->x += entity->xDir;
        entity->y += entity->yDir;

        entity->animationCount -= 1;
        if (entity->animationCount == 0) {
            entity->animationCount = 2;
            entity->animationFrame += 1;
            if (entity->animationFrame == 4) {
                entity->animationFrame = 0;
            }
            moveAndSetAnimationFrame(entity->spriteAddrLo, entity->spriteAddrHi, entity->spriteGraphicLo, entity->spriteGraphicHi,
                entity->x, entity->y, 0, weaponRotation[entity->animationFrame], 0);
        } else {
            moveSprite(entity->spriteAddrLo, entity->spriteAddrHi, entity->x, entity->y);
        }

        // Get the new tile
        entity->currentTileX = (entity->x + 8) >> 4;
        entity->currentTileY = (entity->y + 8) >> 4;

        if (entity->isLob) {
            if (entity->rangedTicks == 0) {
                if (mapStatus[entity->currentTileY][entity->currentTileX] >= GUY_CLAIM) {
                    meleeAttackGuy(mapStatus[entity->currentTileY][entity->currentTileX]-GUY_CLAIM, entity->statsId, entity->stats->ranged, 1);   
                }

                entity->health=0;
                deleteEntityFromList(entity, &entityActiveList);
                toggleSprite(entity->spriteAddrLo, entity->spriteAddrHi, 0);
            }

            entity->xDir = (entity->xLobTarget - (signed short)entity->x) / entity->rangedTicks;
            entity->yDir = (entity->yLobTarget - (signed short)entity->y) / entity->rangedTicks;
            entity->rangedTicks -= 1;
        } else {
            // See if shot hit something
            if (mapStatus[entity->currentTileY][entity->currentTileX] > TILE_FLOOR &&
                mapStatus[entity->currentTileY][entity->currentTileX] != ENTITY_CLAIM) {
                // See if guy is in this tile!
                if (mapStatus[entity->currentTileY][entity->currentTileX] >= GUY_CLAIM) {
                    meleeAttackGuy(mapStatus[entity->currentTileY][entity->currentTileX]-GUY_CLAIM, entity->statsId, entity->stats->ranged, 1);   
                }

                entity->health=0;
                deleteEntityFromList(entity, &entityActiveList);
                toggleSprite(entity->spriteAddrLo, entity->spriteAddrHi, 0);
            }
        }

        return;
    }

    // Does the entity make ranged attacks?
    if (entity->stats->ranged > 0) {
        if (entity->rangedTicks == 0) {
            if (entity->stats->lob) {
                entity->rangedTicks = entity->stats->rangedRate;

                // See where target will be in 1 sec
                xTemp = (signed short)guy->x + (signed short)(guy->pressedX * playerMoveChunks[guy->characterType] * LOB_PLAYER_CHUNK);
                yTemp = (signed short)guy->y + (signed short)(guy->pressedY * playerMoveChunks[guy->characterType] * LOB_PLAYER_CHUNK);

                distX = abs(xTemp - entity->x);
                distY = abs(yTemp - entity->y);

                // If a reachable tile...
                if (xTemp > 0 && yTemp > 0 && distX<=LOB_PIXEL_MAX && distY<=LOB_PIXEL_MAX) {
                    for (i=0; i < ENTITY_COUNT; i++) {
                        if (entityList[i].health == 0) {
                            createEntity(TILE_ENTITY_START+entity->entityTypeId, i, entity->currentTileX, entity->currentTileY);
                            entityList[i].tileId = MONSTER_PROJECTILE_TILE + (entity->entityTypeId>>1); // Move to the shot tile for this entity
                            entityList[i].isShot = 1; 
                            entityList[i].isLob = 1;
                            entityList[i].animationCount = 2;
                            entityList[i].xDir = ((signed short)xTemp - ((signed short)entityList[i].x + 8)) / LOB_MOVE_TICKS;
                            entityList[i].yDir = ((signed short)yTemp - ((signed short)entityList[i].y + 8)) / LOB_MOVE_TICKS;
                            entityList[i].xLobTarget= xTemp;
                            entityList[i].yLobTarget= yTemp;
                            entityList[i].rangedTicks = LOB_MOVE_TICKS;
                            addNewEntityToList(&entityList[i], &entitySleepList);
                            moveAndSetAnimationFrame(entityList[i].spriteAddrLo, entityList[i].spriteAddrHi, entityList[i].spriteGraphicLo, entityList[i].spriteGraphicHi,
                                entityList[i].x, entityList[i].y, 0, 0, 0);
                            break;
                        }
                    }
                }

                return;
                
            } else {
                distX = abs(guyTileX - entity->currentTileX);
                distY = abs(guyTileY - entity->currentTileY);

                // Only shoot if lined up
                if (distX == 0 || distY == 0 || distX == distY) {
                    entity->rangedTicks = entity->stats->rangedRate;

                    for (i=0; i < ENTITY_COUNT; i++) {
                        if (entityList[i].health == 0) {
                            newTileX = distX == 0 ? entity->currentTileX : entity->currentTileX > guyTileX ? entity->currentTileX-1 : entity->currentTileX+1;
                            newTileY = distY == 0 ? entity->currentTileY : entity->currentTileY > guyTileY ? entity->currentTileY-1 : entity->currentTileY+1;

                            createEntity(TILE_ENTITY_START+entity->entityTypeId, i, newTileX, newTileY);
                            entityList[i].tileId = MONSTER_PROJECTILE_TILE + (entity->entityTypeId>>1);
                            entityList[i].isShot = 1;
                            entityList[i].animationCount = 2;
                            entityList[i].xDir = distX == 0 ? 0 : entity->currentTileX > guyTileX ? -RANGED_SPEED : RANGED_SPEED;
                            entityList[i].yDir = distY == 0 ? 0 : entity->currentTileY > guyTileY ? -RANGED_SPEED : RANGED_SPEED;
                            addNewEntityToList(&entityList[i], &entitySleepList);
                            moveAndSetAnimationFrame(entityList[i].spriteAddrLo, entityList[i].spriteAddrHi, entityList[i].spriteGraphicLo, entityList[i].spriteGraphicHi,
                                entityList[i].x, entityList[i].y, 0, 0, 0);
                            break;
                        }
                    }

                    return;
                }
            }
        }

        if (entity->rangedTicks>0) {
            entity->rangedTicks -= 1;
        }
    }

    if (entity->isGenerator) {
        if (entity->nextSpawn > 0) {
            entity->nextSpawn -= 1;
        } else if (activeEntityCount < ENTITY_COUNT_LIMIT && totalEntityCount < ENTITY_COUNT) {
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
            moveAndSetAnimationFrame(entity->spriteAddrLo, entity->spriteAddrHi, entity->spriteGraphicLo, entity->spriteGraphicHi,
                entity->x, entity->y, 0, 0, 0);
        } else {
            moveSprite(entity->spriteAddrLo, entity->spriteAddrHi, entity->x, entity->y);
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

            meleeAttackGuy(attack-GUY_CLAIM, entity->statsId, entity->stats->melee[entity->statsId], 1);
            entity->animationChange = 1;
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

                meleeAttackGuy(mapStatus[newTileY][newTileX]-GUY_CLAIM, entity->statsId, entity->stats->melee[entity->statsId], 1);
                entity->animationChange = 1;
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

    if (entity->animationChange || entity->x != prevX || entity->y != prevY) {
        if (entity->animationChange) {
            entity->animationChange = 0;

            if (entity->animationCount == 0) {
                entity->animationFrame = ANIMATION_FRAME_COUNT; // Attack animation frame
                entity->animationCount = ANIMATION_FRAME_SPEED*2;
            } else if (entity->animationCount == ANIMATION_FRAME_SPEED) {
                // Back to basic frame
                entity->animationFrame = 0;
            }
            entity->animationCount -= 1;
            moveAndSetAnimationFrame(entity->spriteAddrLo, entity->spriteAddrHi, entity->spriteGraphicLo, entity->spriteGraphicHi,
                entity->x, entity->y, entity->animationFrame, entity->facingX, 0);
            needsMove = 0;
        } else if (entity->animationCount == 0) {
            entity->animationCount = ANIMATION_FRAME_SPEED;
            if (entity->animationFrame >= ANIMATION_FRAME_COUNT-1) {
                entity->animationFrame = 0;
            } else {
                entity->animationFrame += 1;
            }

            moveAndSetAnimationFrame(entity->spriteAddrLo, entity->spriteAddrHi, entity->spriteGraphicLo, entity->spriteGraphicHi,
                entity->x, entity->y, entity->animationFrame, entity->facingX, 0);
            needsMove = 0;
        } else {
            entity->animationCount -= 1;
        }
    }

    if (needsMove) {
        moveSprite(entity->spriteAddrLo, entity->spriteAddrHi, entity->x, entity->y);
    }
}

void useScrollOnEntities(unsigned char playerId) {
    Entity *entity, *nextEntity;

    entity = entityActiveList;

    soundPlaySFX(SOUND_SFX_SCROLL, playerId);
    RAM_BANK = CODE_BANK;
    flashLayer1();
    RAM_BANK = MAP_BANK;
    
    while(entity) {
        nextEntity = entity->next;
        attackEntity(playerId, entity, players[playerId].hasBoosts[BOOST_ID_MAGIC]
            ? players[playerId].boostedStats->scrollDamage
            : players[playerId].stats->scrollDamage);
        entity = nextEntity;
    }
}
