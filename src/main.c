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

    if (JOY_LEFT(joy)) {
        if (guy.x >= speed) {
            prevX = guy.x;
            guy.x-= speed;
        }
    } else if (JOY_RIGHT(joy)) {
        if (guy.x <= GUY_MAX) {
            prevX = guy.x;
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

    if (JOY_UP(joy)) {
        if (guy.y >= speed) {
            prevY = guy.y;
            guy.y-= speed;
        }
    } else if (JOY_DOWN(joy)) {
         if (guy.y <= GUY_MAX) {
            prevY = guy.y;
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
            entity->health = 0;
            mapStatus[entity->currentTileY][entity->currentTileX] = TILE_FLOOR;
            if (entity->hasTarget) {
                mapStatus[entity->targetTileY][entity->targetTileX] = TILE_FLOOR;
            }
            toggleEntity(entity->spriteId, 0);
            deleteEntityFromList(entity, &entityActiveList);
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

    while(1) {
        moveGuy(count == 0 ? GUY_SPEED_1 : GUY_SPEED_2);

        scrollX = guy.x-112;
        scrollY = guy.y-112;

        VERA.layer0.vscroll = scrollY;
        // VERA.layer1.vscroll = scrollY;
        VERA.layer0.hscroll = scrollX;
        // VERA.layer1.hscroll = scrollX;

        moveSpriteId(0, guy.x, guy.y, scrollX, scrollY);

        if (count == 0) {
            // activation/deactivation phase
            activateEntities(guy.currentTileX, guy.currentTileY);
            deactivateEntities(guy.currentTileX, guy.currentTileY);
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

        wait();
    }
}