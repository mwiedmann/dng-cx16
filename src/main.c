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
    unsigned short prevX=0, prevY=0;
    unsigned char tile, tempTileX, tempTileY;
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

    if (JOY_LEFT(joy)) {
        if (guy.x >= speed) {
            guy.x-= speed;
        }
    } else if (JOY_RIGHT(joy)) {
        if (guy.x <= GUY_MAX) {
            guy.x+= speed;
        }
    }

    tempTileX = (guy.x+8)>>4;
    if (tempTileX != guy.currentTileX) {
        // Check if new tile is open...move back if not
        tile = mapStatus[guy.currentTileY][tempTileX];
        if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
            guy.x = prevX;
            tempTileX = guy.currentTileX;
        }
    }

    prevY = guy.y;

    if (JOY_UP(joy)) {
        if (guy.y >= speed) {      
            guy.y-= speed;
        }
    } else if (JOY_DOWN(joy)) {
         if (guy.y <= GUY_MAX) {
            guy.y+= speed;
        }
    }

    tempTileY = (guy.y+8)>>4;

    if (tempTileY != guy.currentTileY) {
        // Check if new tile is open...move back if not
        tile = mapStatus[tempTileY][guy.currentTileX];
        if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
            guy.y = prevY;
            tempTileY = guy.currentTileY;
        }
    }

    // See if on an Entity's tile...if so, attack it!
    tile = mapStatus[tempTileY][tempTileX];
    if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
        entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
        if (entity) {
            meleeAttackEntity(entity);
            if (entity->health == 0) {
                mapStatus[entity->currentTileY][entity->currentTileX] = TILE_FLOOR;
                if (entity->hasTarget) {
                    mapStatus[entity->targetTileY][entity->targetTileX] = TILE_FLOOR;
                }
                toggleEntity(entity->spriteId, 0);
                deleteEntityFromList(entity, &entityActiveList);
            } else {
                // Entity not dead yet...guy doesn't move
                guy.x = prevX;
                guy.y = prevY;
                tempTileX = guy.currentTileX;
                tempTileY = guy.currentTileY;
            }
        }
    }

    // See if guy has moved tiles
    if (guy.currentTileX != tempTileX || guy.currentTileY != tempTileY) {
        // Open up the previous tile
        mapStatus[guy.currentTileY][guy.currentTileX] = TILE_FLOOR;

        guy.currentTileX = tempTileX;
        guy.currentTileY = tempTileY;
    }

    // Stamp the current tile with the guy
    mapStatus[guy.currentTileY][guy.currentTileX] = GUY_CLAIM;
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

    guy.health = 60;

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

        moveSpriteId(0, guy.x, guy.y, scrollX, scrollY);

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