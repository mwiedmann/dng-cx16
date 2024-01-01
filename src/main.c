#include <cx16.h>
#include <joystick.h>
#include <stdio.h>

#include "config.h"
#include "tiles.h"
#include "map.h"
#include "sprites.h"
#include "ai.h"
#include "wait.h"
#include "globals.h"
#include "joy.h"
#include "list.h"
#include "utils.h"

unsigned char weaponRotation[4] = {0, 1, 3, 2};
short scrollX, scrollY;

void setGuyDirection(unsigned char playerId) {
    unsigned char joy;

    joy = joy_read(0);

    players[playerId].pressedX = 0;
    players[playerId].pressedY = 0;
    players[playerId].pressedShoot = 0;

    if (JOY_LEFT(joy)) {
        players[playerId].pressedX = -1;
    } else if (JOY_RIGHT(joy)) {
        players[playerId].pressedX = 1;
    }

    if (JOY_UP(joy)) {
        players[playerId].pressedY = -1;
    } else if (JOY_DOWN(joy)) {
        players[playerId].pressedY = 1;
    }

    if (JOY_BTN_1(joy)) {
        players[playerId].pressedShoot = 1;
    }

    if (JOY_BTN_2(joy)) {
        if (players[playerId].pressedScroll == 0) {
            players[playerId].pressedScroll = 1;
        }
    } else {
        players[playerId].pressedScroll = 0;
    }
}

unsigned char tryTile(unsigned char playerId, unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY) {
    unsigned char tile = mapStatus[toY][toX];
    signed char i;

    if (tile == TILE_KEY) {
        players[playerId].keys += 1;
        players[playerId].score += 100;
        mapStatus[toY][toX] = TILE_FLOOR;
        copyTile(fromX, fromY, toX, toY);
    } if (tile == TILE_CHEST) {
        players[playerId].gold += 500;
        players[playerId].score += 500;
        mapStatus[toY][toX] = TILE_FLOOR;
        copyTile(fromX, fromY, toX, toY);
    } else if (tile == TILE_SCROLL) {
        players[playerId].scrolls += 1;
        players[playerId].score += 250;
        mapStatus[toY][toX] = TILE_FLOOR;
        copyTile(fromX, fromY, toX, toY);
        return 0;
    } else if (tile == TILE_DOOR && players[playerId].keys > 0) {
        players[playerId].keys -= 1;
        mapStatus[toY][toX] = TILE_FLOOR;
        copyTile(fromX, fromY, toX, toY);

        i=1;
        while(mapStatus[toY-i][toX] == TILE_DOOR) {
            mapStatus[toY-i][toX] = TILE_FLOOR;
            copyTile(fromX, fromY, toX, toY-i);
            i++;
        }

        i=1;
        while(mapStatus[toY+i][toX] == TILE_DOOR) {
            mapStatus[toY+i][toX] = TILE_FLOOR;
            copyTile(fromX, fromY, toX, toY+i);
            i++;
        }

        i=1;
        while(mapStatus[toY][toX-i] == TILE_DOOR) {
            mapStatus[toY][toX-i] = TILE_FLOOR;
            copyTile(fromX, fromY, toX-i, toY);
            i++;
        }

        i=1;
        while(mapStatus[toY][toX+i] == TILE_DOOR) {
            mapStatus[toY][toX+i] = TILE_FLOOR;
            copyTile(fromX, fromY, toX+i, toY);
            i++;
        }
        
    } else if (tile == TILE_EXIT) {
        players[playerId].exit = 1;
        return 0;
    }

    if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
        return 1;
    }

    return 0;
}

void moveGuy(unsigned char playerId, unsigned char speed) {
    unsigned short prevX, prevY, shot = 0;
    unsigned char tile, tempTileX, tempTileY;
    signed char dirX = 0, dirY = 0;
    Entity *entity;

    players[playerId].currentTileX = (players[playerId].x+8)>>4;
    players[playerId].currentTileY = (players[playerId].y+8)>>4;

    // Pause
    // if (JOY_BTN_1(joy)) {
    //     waitForRelease();
    //     waitForButtonPress();
    // }
    prevX = players[playerId].x;
    prevY = players[playerId].y;

    if (players[playerId].pressedX == -1) {
        dirX = -1;
        players[playerId].x -= speed;
        players[playerId].facingX = 1;
    } else if (players[playerId].pressedX == 1) {
        dirX = 1;
        players[playerId].x += speed;
        players[playerId].facingX = 0;
    }

    if (dirX == 1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, (players[playerId].x+15)>>4, players[playerId].y>>4)) {
            dirX = 0;
            players[playerId].x = prevX;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, (players[playerId].x+15)>>4, (players[playerId].y+15)>>4)) {
                dirX = 0;
                players[playerId].x = prevX;
            }
        }
    } else if (dirX == -1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x>>4, players[playerId].y>>4)) {
            dirX = 0;
            players[playerId].x = prevX;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x>>4, (players[playerId].y+15)>>4)) {
                dirX = 0;
                players[playerId].x = prevX;
            }
        }
    }

    if (players[playerId].pressedY == -1) {
        dirY = -1;
        players[playerId].y -= speed;
    } else if (players[playerId].pressedY == 1) {
        dirY = 1;
        players[playerId].y += speed;
    }

    if (dirY == 1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, (players[playerId].x+15)>>4, (players[playerId].y+15)>>4)) {
            dirY = 0;
            players[playerId].y = prevY;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x>>4, (players[playerId].y+15)>>4)) {
                dirY = 0;
                players[playerId].y = prevY;
            }
        }
    } else if (dirY == -1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x>>4, players[playerId].y>>4)) {
            dirY = 0;
            players[playerId].y = prevY;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, (players[playerId].x+15)>>4, players[playerId].y>>4)) {
                dirY = 0;
                players[playerId].y = prevY;
            }
        }
    }

    // Set the guy's aiming direction
    if (players[playerId].pressedX != 0 && players[playerId].pressedY == 0) {
        players[playerId].aimX = players[playerId].pressedX;
        players[playerId].aimY = 0;
    } else if (players[playerId].pressedX == 0 && players[playerId].pressedY != 0) {
        players[playerId].aimX = 0;
        players[playerId].aimY = players[playerId].pressedY;
    } else if (players[playerId].pressedX != 0 && players[playerId].pressedY != 0) {
        players[playerId].aimX = players[playerId].pressedX;
        players[playerId].aimY = players[playerId].pressedY;
    } // Other cases just leave the facing as it is


    // Check if attacking
    tile = mapStatus[((players[playerId].y+8)+(dirY*8))>>4][((players[playerId].x+8)+(dirX*8))>>4];
    if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
        if (players[playerId].ticksUntilNextMelee == 0) {
            entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
            if (entity) {
                players[playerId].ticksUntilNextMelee = players[playerId].stats->ticksToMelee + 1;
                attackEntity(playerId, entity, players[playerId].stats->meleeDamage);
                if (entity->health > 0) {
                    // Entity not dead yet...guy doesn't move
                    players[playerId].x = prevX;
                    players[playerId].y = prevY;
                }
            }
        } else {
            // Can't attack yet...move back
            players[playerId].x = prevX;
            players[playerId].y = prevY;
        }
    }
    
    if (players[playerId].ticksUntilNextMelee > 0) {
        players[playerId].ticksUntilNextMelee -= 1;
    }

    tempTileX = (players[playerId].x+8)>>4;
    tempTileY = (players[playerId].y+8)>>4;

    // See if guy has moved tiles
    if (players[playerId].currentTileX != tempTileX || players[playerId].currentTileY != tempTileY) {
        // Open up the previous tile
        mapStatus[players[playerId].currentTileY][players[playerId].currentTileX] = TILE_FLOOR;

        players[playerId].currentTileX = tempTileX;
        players[playerId].currentTileY = tempTileY;
    }

    // Stamp the current tile with the guy
    mapStatus[players[playerId].currentTileY][players[playerId].currentTileX] = GUY_CLAIM-playerId;

    if (players[playerId].pressedShoot && !weapons[playerId].visible && players[playerId].ticksUntilNextShot == 0) {
        players[playerId].ticksUntilNextShot = players[playerId].stats->ticksToRanged;
        weapons[playerId].x = players[playerId].x;
        weapons[playerId].y = players[playerId].y;
        weapons[playerId].visible = 1;
        weapons[playerId].dirX = players[playerId].aimX;
        weapons[playerId].dirY = players[playerId].aimY;
        weapons[playerId].animationCount = WEAPON_ROTATION_SPEED;
        weapons[playerId].animationFrame = 0;

        toggleWeapon(playerId, 1);

        shot = 1;
    } else {
        if (players[playerId].ticksUntilNextShot > 0) {
            players[playerId].ticksUntilNextShot -= 1;
        }
    }

    // See if player used scroll
    if (players[playerId].pressedScroll && players[playerId].scrolls > 0) {
        players[playerId].scrolls -= 1;
        useScrollOnEntities(playerId);
    }

    // Animate the guy
    if (shot || players[playerId].x != prevX || players[playerId].y != prevY || players[playerId].animationFrame == ANIMATION_FRAME_COUNT) {
        if (shot) {
            players[playerId].animationChange = 1;
            players[playerId].animationCount = ANIMATION_FRAME_SPEED;
            players[playerId].animationFrame = ANIMATION_FRAME_COUNT;
        } 
        
        if (players[playerId].animationCount == 0) {
            players[playerId].animationChange = 1;
            players[playerId].animationCount = ANIMATION_FRAME_SPEED;
            if (players[playerId].animationFrame >= ANIMATION_FRAME_COUNT-1) {
                players[playerId].animationFrame = 0;
            } else {
                players[playerId].animationFrame += 1;
            }
        } else {
            players[playerId].animationCount -= 1;
        }
    }
}

// void main() {
//     unsigned char x,y;

//     loadFileToBankedRAM("l0.bin", MAP_BANK, 0);

//     // mapStatus = (unsigned char (*)[MAP_MAX])BANK_RAM;

//     // printf("%X\n", mapStatus);

//     for (y=0; y<2; y++) {
//         for (x=0; x<10; x++) {
//             printf("%u ", mapStatus[y][x]);
//         }
//         printf("\n");
//     }
// }

void setupPlayer(unsigned char playerId, enum Character characterType) {
    players[playerId].active = 1;
    players[playerId].characterType = characterType;
    players[playerId].stats =  playerStatsByType[characterType];
    players[playerId].health = players[playerId].stats->startingHealth;
    players[playerId].animationCount = ANIMATION_FRAME_SPEED;
    players[playerId].animationFrame = 0;
    players[playerId].animationTile = GUY_TILE_START + (playerId*4) + (characterType*8);
    players[playerId].weaponTile = GUY_TILE_START + (playerId*4) + (characterType*8) + 5;
    players[playerId].ticksUntilNextMelee = 0;
    players[playerId].ticksUntilNextShot = 0;
    players[playerId].shooting = 0;
    players[playerId].aimX = 1;
    players[playerId].aimY = 0;
    players[playerId].facingX = 0;
    players[playerId].score = 0;
    players[playerId].gold = 0;
    players[playerId].keys = 0;
    players[playerId].scrolls = 0;
    players[playerId].exit = 0;
    players[playerId].animationChange = 1; // Trigger immediate animation
}

void moveWeapon(unsigned char playerId) {
    unsigned char tile;
    Entity *entity;

    if (weapons[playerId].visible) {
        weapons[playerId].animationCount -= 1;
        if (weapons[playerId].animationCount == 0) {
            weapons[playerId].animationCount = WEAPON_ROTATION_SPEED;
            weapons[playerId].animationFrame += 1;
            if (weapons[playerId].animationFrame == 4) {
                weapons[playerId].animationFrame = 0;
            }
        }

        // We check before and after the move because of tile boundary edge cases
        // Probably a better way but tile checking is really fast...so, maybe this is ok
        tile = mapStatus[(weapons[playerId].y+8)>>4][(weapons[playerId].x+8)>>4];
        if (tile != TILE_FLOOR && tile != GUY_CLAIM && tile != GUY_CLAIM-1 && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleWeapon(playerId, 0);

            if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    attackEntity(playerId, entity, players[playerId].stats->rangedDamage);
                }
            }
            return;
        }

        weapons[playerId].x += weapons[playerId].dirX * WEAPON_SPEED;
        weapons[playerId].y += weapons[playerId].dirY * WEAPON_SPEED;

        // See if weapon is off screen
        if (weapons[playerId].x >= scrollX + SCROLL_PIXEL_SIZE || weapons[playerId].x+16 <= scrollX || weapons[playerId].y >= scrollY + SCROLL_PIXEL_SIZE || weapons[playerId].y+16 <= scrollY) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleWeapon(playerId, 0);
            return;
        }

        moveAndSetAnimationFrame(WEAPON_SPRITE_ID_START+playerId, weapons[playerId].x, weapons[playerId].y, scrollX, scrollY, players[playerId].weaponTile, 0, weaponRotation[weapons[playerId].animationFrame]);

        // Check if hit something after the move
        tile = mapStatus[(weapons[playerId].y+8)>>4][(weapons[playerId].x+8)>>4];
        if (tile != TILE_FLOOR && tile != GUY_CLAIM && tile != GUY_CLAIM-1 && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleWeapon(playerId, 0);

            if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    attackEntity(playerId, entity, players[playerId].stats->rangedDamage);
                }
            }
        }
    }
}

void main() {
    unsigned char count = 0, load, level=0, exitLevel=0, gameOver=0, i;
    unsigned char inputTicks = 0;
    Entity *entity;

    init();
    initTiles();
    spritesConfig();
    clearLayers();

    // Only player 0 for now
    players[1].active = 0;
    players[1].characterType = DRUID;

    setupPlayer(0, RANGER);

    drawOverlayBackground();
    
    while(!gameOver) {
        createMapStatus(level);
        drawMap(level);

        while(!exitLevel && !gameOver) {
            // Get joystick input only periodically
            if (inputTicks == 4) {
                for (i=0; i<NUM_PLAYERS; i++) {
                    if (!players[i].active) {
                        continue;
                    }
                    setGuyDirection(i);
                }
                inputTicks = 0;
            } else {
                inputTicks++;
            }

            for (i=0; i<NUM_PLAYERS; i++) {
                if (!players[i].active) {
                    continue;
                }
                moveGuy(i, players[i].stats->speeds[count]);
            }

            // TODO: Scroll between 2 players if both active
            // Just follow P0 for now
            scrollX = players[0].x-112;
            if (scrollX < 0) {
                scrollX = 0;
            } else if (scrollX > maxMapX) {
                scrollX = maxMapX;
            }

            scrollY = players[0].y-112;
            if (scrollY < 0) {
                scrollY = 0;
            } else if (scrollY > maxMapY) {
                scrollY = maxMapY;
            }
            
            VERA.layer0.vscroll = scrollY;
            VERA.layer0.hscroll = scrollX;

            // Only set his animation frame if needed (this is more expensive)
            // Otherwise just move him
            for (i=0; i<NUM_PLAYERS; i++) {
                if (!players[i].active) {
                    continue;
                }

                if (players[i].animationChange) {
                    players[i].animationChange = 0;
                    moveAndSetAnimationFrame(i, players[i].x, players[i].y, scrollX, scrollY, players[i].animationTile, players[i].animationFrame, players[i].facingX);
                } else {
                    moveSpriteId(i, players[i].x, players[i].y, scrollX, scrollY);
                }

                moveWeapon(i);
            }

            if (count == 0) {
                // activation/deactivation phase
                activateEntities(scrollX, scrollY);
                deactivateEntities(scrollX, scrollY);
                tempActiveToActiveEntities();

                // Move "some" active entities
                // Try to split the load a bit
                entity = entityActiveList;
                load=0;
                while(entity && load<10) {
                    moveEntity(entity, players[0].currentTileX, players[0].currentTileY, scrollX, scrollY);   
                    entity->movedPrevTick=1;
                    entity = entity->next;
                    load++;
                };
            } else {
                // Move active entities phase
                entity = entityActiveList;

                while(entity) {
                    if (!entity->movedPrevTick) {
                        moveEntity(entity, players[0].currentTileX, players[0].currentTileY, scrollX, scrollY);   
                    } else {
                        entity->movedPrevTick = 0;
                    }
                    entity = entity->next;
                };

                // TODO: Only need to update this periodically
                updateOverlay();
            }

            count++;
            if (count == 2) {
                count = 0;
            }

            exitLevel = 1;
            gameOver = 1;
            for (i=0; i<NUM_PLAYERS; i++) {
                if (players[i].active) {
                    gameOver = 0;
                    if (!players[i].exit) {
                        exitLevel = 0;
                    }
                }
            }
            wait();
        }

        level++;

        for (i=0; i<NUM_PLAYERS; i++) {
            players[i].exit = 0;
        }
    }
}