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

void moveGuy() {
    unsigned char joy;
    unsigned short prevX=0, prevY=0;
    unsigned char tile, tempTileX, tempTileY;

    guy.currentTileX = (guy.x+8)>>4;
    guy.currentTileY = (guy.y+8)>>4;

    mapStatus[guy.currentTileY][guy.currentTileX] = GUY_CLAIM;

    joy = joy_read(0);

    // Pause
    if (JOY_BTN_1(joy)) {
        waitForRelease();
        waitForButtonPress();
    }

    if (JOY_LEFT(joy)) {
        if (guy.x >= GUY_SPEED) {
            prevX = guy.x;
            guy.x-= GUY_SPEED;
        }
    } else if (JOY_RIGHT(joy)) {
        if (guy.x <= GUY_MAX) {
            prevX = guy.x;
            guy.x+= GUY_SPEED;
        }
    }

    tempTileX = (guy.x+8)>>4;

    // Check if new tile is open...move back if not
    tile = mapStatus[(guy.y+8)>>4][tempTileX];
    if (tile > 0 && tile <= ENTITY_CLAIM) {
        guy.x = prevX;
        tempTileX = guy.currentTileX;
    }

    if (JOY_UP(joy)) {
        if (guy.y >= GUY_SPEED) {
            prevY = guy.y;
            guy.y-= GUY_SPEED;
        }
    } else if (JOY_DOWN(joy)) {
         if (guy.y <= GUY_MAX) {
            prevY = guy.y;
            guy.y+= GUY_SPEED;
        }
    }

    tempTileY = (guy.y+8)>>4;

    // Check if new tile is open...move back if not
    tile = mapStatus[tempTileY][(guy.x+8)>>4];
    if (tile > 0 && tile <= ENTITY_CLAIM) {
        guy.y = prevY;
        tempTileY = guy.currentTileY;
    }

    // See if guy has moved tiles
    if (guy.currentTileX != tempTileX || guy.currentTileY != tempTileY) {
        // Open up the previous tile
        mapStatus[guy.currentTileY][guy.currentTileX] = TILE_EMPTY;

        // Block the current tile
        mapStatus[tempTileY][tempTileX] = GUY_CLAIM;

        guy.currentTileX = tempTileX;
        guy.currentTileY = tempTileY;
    }
}

// void main() {
//     createMapStatus();

//     printList("Sleep", entitySleepList);
//     printList("Active", entityActiveList);
//     printList("Temp", entityTempActiveList);

//     activateEntities(20, 0);
//     deactivateEntities(20, 0);
//     tempActiveToActiveEntities();

//     printList("Sleep", entitySleepList);
//     printList("Active", entityActiveList);
//     printList("Temp", entityTempActiveList);

//     activateEntities(20, 0);
//     deactivateEntities(20, 0);
//     tempActiveToActiveEntities();

//     printList("Sleep", entitySleepList);
//     printList("Active", entityActiveList);
//     printList("Temp", entityTempActiveList);
// }

void main() {
    unsigned char count = 0;
    short scrollX, scrollY;
    Entity *entity;

    init();
    initTiles();
    spritesConfig();
    clearLayers();
    createMapStatus();
    drawMap();

    while(1) {
        moveGuy();

        scrollX = guy.x-160;
        scrollY = guy.y-120;

        VERA.layer0.vscroll = scrollY;
        VERA.layer1.vscroll = scrollY;
        VERA.layer0.hscroll = scrollX;
        VERA.layer1.hscroll = scrollX;

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