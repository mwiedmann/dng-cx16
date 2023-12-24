#include <cx16.h>

#include "config.h"
#include "tiles.h"
#include "map.h"
#include "globals.h"
#include "utils.h"

void createMapStatus() {
    unsigned char x,y,i;
    Entity *lastEntity = 0;

    loadFileToBankedRAM("l0.bin", MAP_BANK, 0);
    BANK_NUM = MAP_BANK;

    entitySleepList = 0;

    for (y=0,i=0; y<MAP_MAX; y++) {
        for (x=0; x<MAP_MAX; x++) {
            if (mapStatus[y][x] == TILE_ENTITY /* Entity */) {
                // Create an entity at this tile
                entityList[i].spriteId = i+1;
                entityList[i].x = x * 16;
                entityList[i].y = y * 16;
                entityList[i].hasTarget = 0;
                entityList[i].visible = 0;
                entityList[i].health =  60; // Every 3rd entity is dead
                entityList[i].currentTileX = x;
                entityList[i].currentTileY = y;
                entityList[i].prev = lastEntity; // For the LL, prev entity
                entityList[i].next = 0;

                // If we have a lastEntity (not at beginning of list)
                // Point it's 'next' to this entity
                if (lastEntity) {
                    lastEntity->next = &entityList[i];
                }

                // Move the lastEntity to be this new entity (for the next iteration)
                lastEntity = &entityList[i];

                mapStatus[y][x] = ENTITY_TILE_START + i+1;
                i++;
            } else if (mapStatus[y][x] == TILE_GUY /* Guy Start*/) {
                mapStatus[y][x] = 0;
                guy.x = x * 16;
                guy.y = y * 16;
            }
        }
    }

    // This will put all the entities in the sleeping list
    entitySleepList = &entityList[0];
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
                    : TILE_FLOOR;
            } else {
                VERA.data0 = 0;
            }
            VERA.data0 = 0;
        }
    }
}