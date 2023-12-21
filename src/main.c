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

void main() {
    unsigned char i;
    unsigned char count = 0;
    short scrollX, scrollY;

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

        // Activate every entity
        // This is too slow when there is a large number of entities
        // for (i=0; i<ENTITY_COUNT; i++) {
        //     moveEntity(i, (guy.x+8) >> 4, (guy.y+8) >> 4);
        // }

        // Alternate between activating even OR odd entities
        // This alternates frames and make the game run more smoothly
        // We move the Entities twice as far to make up for their skipped frames
        for (i=count; i<ENTITY_COUNT; i+=AI_SKIP) {
            if (entityList[i].health > 0) {
                moveEntity(i, guy.currentTileX, guy.currentTileY, scrollX, scrollY);
            }
        }

        count++;
        if (count == AI_SKIP) {
            count = 0;
        }

        wait();
    }
}