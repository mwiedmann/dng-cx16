#include <cx16.h>

#include "config.h"
#include "tiles.h"
#include "map.h"
#include "globals.h"

void createMapStatus() {
    unsigned char x,y,i;

    for (y=0,i=0; y<MAP_MAX; y++) {
        for (x=0; x<MAP_MAX; x++) {
            mapStatus[y][x] = y % 4 == 0 && x % 4 == 0 ? TILE_SOLID : TILE_EMPTY;

            if (i < ENTITY_COUNT && x < 39 && mapStatus[y][x] == 0) {
                entityList[i].x = x * 16;
                entityList[i].y = y * 16;
                entityList[i].hasTarget = 0;
                entityList[i].visible = 0;
                entityList[i].health =  i % 3 == 0 ? 0 : 1; // Every 3rd entity is dead
                entityList[i].currentTileX = x;
                entityList[i].currentTileY = y;
                mapStatus[y][x] = ENTITY_TILE_START + i;
                i++;
            }
        }
    }
}

void drawMap() {
    unsigned char x,y;

    // Point to the MapBase address so we can write to VRAM
    VERA.address = L0_MAPBASE_ADDR;
    VERA.address_hi = L0_MAPBASE_ADDR>>16;

    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    for (y=0; y<MAPBASE_TILE_WIDTH; y++) {
        for (x=0; x<MAPBASE_TILE_HEIGHT; x++) {
            if (x < MAP_MAX && y < MAP_MAX) {
                VERA.data0 = mapStatus[y][x] > 0 && mapStatus[y][x] < ENTITY_TILE_START
                    ? mapStatus[y][x]
                    : 0;
            } else {
                VERA.data0 = 0;
            }
            VERA.data0 = 0;
        }
    }
}