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

void attackEntity(unsigned char playerId, Entity *entity, unsigned char damage) {
    // Can't attack shots
    if (entity->isShot) {
        return;
    }

    if (entity->health > damage) {
        entity->health -= damage;
        if (!entity->wasHit) {
            entity->wasHit = ENTITY_HIT_ANIM_FRAMES;
            entity->animationChange = 1;
        }
    } else {
        overlayChanged = 1;
        players[playerId].score += entity->points;
        entity->health = 0;
        
        // If the entity has a target...clear the start/target tiles
        if (entity->hasTarget) {
            mapStatus[entity->startTileY][entity->startTileX] = TILE_FLOOR;

            // Players can stomp on ENTITY_CLAIM tiles...so make sure it is still claimed before clearing it
            if (mapStatus[entity->targetTileY][entity->targetTileX] >= ENTITY_CLAIM_START &&
                mapStatus[entity->targetTileY][entity->targetTileX] <= ENTITY_CLAIM_END) {
                mapStatus[entity->targetTileY][entity->targetTileX] = TILE_FLOOR;
            }
        } else {
            // Entity had no target so just clear it's current tile
            mapStatus[entity->currentTileY][entity->currentTileX] = TILE_FLOOR;
        }

        toggleSprite(entity->spriteAddrLo, entity->spriteAddrHi, 0);
        deleteEntityFromList(entity, &entityActiveList);
    }
}

void meleeAttackGuy(unsigned char playerId, unsigned char statsId, unsigned char dmg, unsigned char showDmg) {
    unsigned char genEntityId;
    signed char adjustedDmg = dmg - players[playerId].stats->armor[statsId];
    unsigned short newHealth;

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
        newHealth = players[playerId].health - adjustedDmg;

        if (players[playerId].health >= NIGH_HEALTH && newHealth < NIGH_HEALTH) {
            soundPlayMusic(SOUND_MUSIC_NIGH);
        }

        players[playerId].health = newHealth;
    } else {
        genEntityId = destroyPlayer(playerId);
        // Update the map here because we switched banks
        mapStatus[players[playerId].currentTileY][players[playerId].currentTileX] = ENTITY_TILE_START + (entityList[genEntityId].spriteId - 4);
        
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
            if ((mapStatus[entity->currentTileY][entity->currentTileX] > TILE_FLOOR &&
                mapStatus[entity->currentTileY][entity->currentTileX] < ENTITY_CLAIM_START) ||
                mapStatus[entity->currentTileY][entity->currentTileX] > ENTITY_CLAIM_END) {
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
    if (entity->stats && entity->stats->ranged) {
        if (!entity->rangedTicks) {
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
                            createEntity(TILE_ENTITY_START+entity->entityTypeId, i, entity->currentTileX, entity->currentTileY, 1);
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
                            entity->animationChange = 1;
                            entity->animationCount = 0;
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

                            createEntity(TILE_ENTITY_START+entity->entityTypeId, i, newTileX, newTileY, 1);
                            entityList[i].animationCount = 2;
                            entityList[i].xDir = distX == 0 ? 0 : entity->currentTileX > guyTileX ? -RANGED_SPEED : RANGED_SPEED;
                            entityList[i].yDir = distY == 0 ? 0 : entity->currentTileY > guyTileY ? -RANGED_SPEED : RANGED_SPEED;
                            addNewEntityToList(&entityList[i], &entitySleepList);
                            moveAndSetAnimationFrame(entityList[i].spriteAddrLo, entityList[i].spriteAddrHi, entityList[i].spriteGraphicLo, entityList[i].spriteGraphicHi,
                                entityList[i].x, entityList[i].y, 0, 0, 0);
                            entity->animationChange = 1;
                            entity->animationCount = 0;
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
                        createEntity(TILE_ENTITY_START+entity->entityTypeId, i, newTileX, newTileY, 0);
                        addNewEntityToList(&entityList[i], &entitySleepList);
                        mapStatus[newTileY][newTileX] = ENTITY_TILE_START + (entityList[i].spriteId-4);
                        break;
                    }
                }
            }
        }

        if (entity->animationChange || entity->wasHit) {
            entity->animationChange = 0;
            moveAndSetAnimationFrame(entity->spriteAddrLo, entity->spriteAddrHi, entity->spriteGraphicLo, entity->spriteGraphicHi,
                entity->x, entity->y, 0, 0, entity->wasHit > ENTITY_HIT_ANIM_FRAMES_STOP ? ENTITY_HIT_PAL_JUMP : 0);

            if (entity->wasHit) {
                entity->wasHit-= 1;
            }
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

            if (entity->entityTypeId == 8) {
                demonHitting = 30;
            }
            
            meleeAttackGuy(attack-GUY_CLAIM, entity->statsId, entity->stats->melee[entity->statsId], 1);
            entity->animationChange = 1;
        }

        if (tileXChange !=0 || tileYChange !=0) {
            // Set the new start and target for the entity
            entity->startTileX = entity->currentTileX;
            entity->startTileY = entity->currentTileY;
            entity->targetTileX = entity->currentTileX+tileXChange;
            entity->targetTileY = entity->currentTileY+tileYChange;
            entity->targetTilePixelX = entity->targetTileX * 16;
            entity->targetTilePixelY = entity->targetTileY * 16;

            // Claim the tile and now entity has a target
            mapStatus[entity->targetTileY][entity->targetTileX] = ENTITY_CLAIM_START + (entityList[i].spriteId-4);
            entity->hasTarget = 1;
        }
    } else {
        // Try to move X towards the target
        if (entity->x < entity->targetTilePixelX) {
            tileXChange = 1;
            entity->facingX = 0;
        } else if (entity->x > entity->targetTilePixelX) {
            tileXChange = -1;
            entity->facingX = 1;
        }

        // Try to move Y towards target
        if (entity->y < entity->targetTilePixelY) {
            tileYChange = 1;
        } else if (entity->y > entity->targetTilePixelY) {
            tileYChange = -1;
        }

        // There a bug where the target is reached but not detected
        // This will fix for now.
        if (!tileXChange && !tileYChange) {
            entity->hasTarget = 0;
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
        if (entity->hasTarget && entity->targetTileX == newTileX && entity->targetTileY == newTileY) {
            // See if guy is in this tile...if so, stay still, but attack!
            if (mapStatus[newTileY][newTileX] >= GUY_CLAIM) {
                entity->x = prevX;
                entity->y = prevY;

                if (entity->entityTypeId == 8) {
                    demonHitting = 30;
                }
                meleeAttackGuy(mapStatus[newTileY][newTileX]-GUY_CLAIM, entity->statsId, entity->stats->melee[entity->statsId], 1);
                entity->animationChange = 1;
            } else {
                // Clear the old tile and mark the new tile as blocked
                mapStatus[entity->startTileY][entity->startTileX] = TILE_FLOOR; // Remove target blocker (can be diff) from actual new tile
                mapStatus[entity->targetTileY][entity->targetTileX] = (ENTITY_TILE_START + entity->spriteId-4); // Block new tile
                entity->hasTarget = 0; // Will need new target

                // Update the current tile
                // Be careful to not update currentTile anywhere else, it causes issues related to mapStatus stamping
                // and you end up with dead spaces on the map that entities won't move through. Its a whole thing!
                entity->currentTileX = newTileX;
                entity->currentTileY = newTileY;
            }
        }
    }

    if (entity->wasHit || entity->animationChange || entity->x != prevX || entity->y != prevY) {
        if (entity->wasHit || entity->animationChange) {
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
                entity->x, entity->y, entity->animationFrame, entity->facingX, entity->wasHit > ENTITY_HIT_ANIM_FRAMES_STOP ? ENTITY_HIT_PAL_JUMP : 0);
            needsMove = 0;
        } else if (entity->animationCount == 0) {
            entity->animationCount = ANIMATION_FRAME_SPEED;
            if (entity->animationFrame >= ANIMATION_FRAME_COUNT-1) {
                entity->animationFrame = 0;
            } else {
                entity->animationFrame += 1;
            }

            moveAndSetAnimationFrame(entity->spriteAddrLo, entity->spriteAddrHi, entity->spriteGraphicLo, entity->spriteGraphicHi,
                entity->x, entity->y, entity->animationFrame, entity->facingX, entity->wasHit > ENTITY_HIT_ANIM_FRAMES_STOP ? ENTITY_HIT_PAL_JUMP : 0);
            needsMove = 0;
        } else {
            entity->animationCount -= 1;
        }

        if (entity->wasHit) {
            entity->wasHit-= 1;
        }
    }

    if (needsMove) {
        moveSprite(entity->spriteAddrLo, entity->spriteAddrHi, entity->x, entity->y);
    }
}

void useScrollOnEntities(unsigned char playerId) {
    Entity *entity, *nextEntity;
    unsigned char damage;

    entity = entityActiveList;

    soundPlaySFX(SOUND_SFX_SCROLL, playerId);
    RAM_BANK = CODE_BANK;
    flashLayer1();
    RAM_BANK = MAP_BANK;
    
    while(entity) {
        nextEntity = entity->next;

        // Indestructable entites take more damage from scrolls
        damage = entity->stats->startingHealth == INDESTRUCTABLE_HEALTH
            ? INDESTRUCTABLE_HEALTH
            : players[playerId].hasBoosts[BOOST_ID_MAGIC]
                ? players[playerId].boostedStats->scrollDamage
                : players[playerId].stats->scrollDamage;

        attackEntity(playerId, entity, damage);
        entity = nextEntity;
    }
}
