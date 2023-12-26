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

void moveGuy(unsigned char speed) {
    unsigned char joy;
    unsigned short prevX, prevY;
    unsigned char tile, tempTileX, tempTileY;
    signed char dirX = 0, dirPressX = 0, dirY = 0, dirPressY = 0;
    Entity *entity;

    guy.currentTileX = (guy.x+8)>>4;
    guy.currentTileY = (guy.y+8)>>4;

    joy = joy_read(0);

    // Pause
    // if (JOY_BTN_1(joy)) {
    //     waitForRelease();
    //     waitForButtonPress();
    // }
    prevX = guy.x;
    prevY = guy.y;

    if (JOY_LEFT(joy)) {
        dirX = -1;
        dirPressX = -1;
        guy.x -= speed;
        guy.facing = 1;
    } else if (JOY_RIGHT(joy)) {
        dirX = 1;
        dirPressX = 1;
        guy.x += speed;
        guy.facing = 0;
    }

    if (dirX == 1) {
        // Check if new tile is open...move back if not
        tile = mapStatus[guy.y>>4][(guy.x+15)>>4];
        if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
            dirX = 0;
            guy.x = prevX;
        } else {
            tile = mapStatus[(guy.y+15)>>4][(guy.x+15)>>4];
            if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
                dirX = 0;
                guy.x = prevX;
            }
        }
    } else if (dirX == -1) {
        // Check if new tile is open...move back if not
        tile = mapStatus[guy.y>>4][guy.x>>4];
        if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
            dirX = 0;
            guy.x = prevX;
        } else {
            tile = mapStatus[(guy.y+15)>>4][guy.x>>4];
            if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
                dirX = 0;
                guy.x = prevX;
            }
        }
    }

    if (JOY_UP(joy)) {
        dirY = -1;
        dirPressY = -1;
        guy.y -= speed;
    } else if (JOY_DOWN(joy)) {
        dirY = 1;
        dirPressY = 1;
        guy.y += speed;
    }

    if (dirY == 1) {
        // Check if new tile is open...move back if not
        tile = mapStatus[(guy.y+15)>>4][(guy.x+15)>>4];
        if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
            dirY = 0;
            guy.y = prevY;
        } else {
            tile = mapStatus[(guy.y+15)>>4][guy.x>>4];
            if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
                dirY = 0;
                guy.y = prevY;
            }
        }
    } else if (dirY == -1) {
        // Check if new tile is open...move back if not
        tile = mapStatus[guy.y>>4][guy.x>>4];
        if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
            dirY = 0;
            guy.y = prevY;
        } else {
            tile = mapStatus[guy.y>>4][(guy.x+15)>>4];
            if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
                dirY = 0;
                guy.y = prevY;
            }
        }
    }

    // Check if attacking
    tile = mapStatus[((guy.y+8)+(dirY*8))>>4][((guy.x+8)+(dirX*8))>>4];
    if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
        entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
        if (entity) {
            meleeAttackEntity(entity);
            if (entity->health > 0) {
                // Entity not dead yet...guy doesn't move
                guy.x = prevX;
                guy.y = prevY;
            }
        }
    }

    tempTileX = (guy.x+8)>>4;
    tempTileY = (guy.y+8)>>4;

    // See if guy has moved tiles
    if (guy.currentTileX != tempTileX || guy.currentTileY != tempTileY) {
        // Open up the previous tile
        mapStatus[guy.currentTileY][guy.currentTileX] = TILE_FLOOR;

        guy.currentTileX = tempTileX;
        guy.currentTileY = tempTileY;
    }

    // Stamp the current tile with the guy
    mapStatus[guy.currentTileY][guy.currentTileX] = GUY_CLAIM;

    // Animate the guy
    if (guy.x != prevX || guy.y != prevY) {
        if (guy.animationCount == 0) {
            guy.animationCount = ANIMATION_FRAME_SPEED;
            if (guy.animationFrame == ANIMATION_FRAME_COUNT-1) {
                guy.animationFrame = 0;
            } else {
                guy.animationFrame += 1;
            }
        } else {
            guy.animationCount -= 1;
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

void setupGuy() {
    guy.health = 60;
    guy.animationCount = ANIMATION_FRAME_SPEED;
    guy.animationFrame = 0;
    guy.ticksUntilNextMelee = 0;
    guy.ticksUntilNextShot = 0;
    guy.shooting = 0;
}

void main() {
    unsigned char count = 0;
    short scrollX, scrollY;
    Entity *entity;

    init();
    initTiles();
    spritesConfig();
    clearLayers();
    drawOverlay();
    createMapStatus();
    drawMap();

    setupGuy();

    while(1) {
        moveGuy(count == 0 ? GUY_SPEED_1 : GUY_SPEED_2);

        scrollX = guy.x-112;
        if (scrollX < 0) {
            scrollX = 0;
        } else if (scrollX > MAP_SCROLL_MAX) {
            scrollX = MAP_SCROLL_MAX;
        }

        scrollY = guy.y-112;
        if (scrollY < 0) {
            scrollY = 0;
        } else if (scrollY > MAP_SCROLL_MAX) {
            scrollY = MAP_SCROLL_MAX;
        }
        
        VERA.layer0.vscroll = scrollY;
        // VERA.layer1.vscroll = scrollY;
        VERA.layer0.hscroll = scrollX;
        // VERA.layer1.hscroll = scrollX;

        moveAndSetAnimationFrame(0, guy.x, guy.y, scrollX, scrollY, GUY_TILE, guy.animationFrame, guy.facing);

        // moveSpriteId(0, guy.x, guy.y, scrollX, scrollY);

        if (count == 0) {
            // activation/deactivation phase
            activateEntities(scrollX, scrollY);
            deactivateEntities(scrollX, scrollY);
            tempActiveToActiveEntities();
        } else {
            // Move active entities phase
            entity = entityActiveList;

            while(entity) {
                moveEntity(entity, guy.currentTileX, guy.currentTileY, scrollX, scrollY);   
                entity = entity->next;
            };
        }

        count++;
        if (count == 2) {
            count = 0;
        }

        // Stop
        if (guy.health == 0) {
            while(1);
        }

        wait();
    }
}