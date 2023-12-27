#include <cx16.h>

#include "config.h"
#include "tiles.h"
#include "map.h"
#include "globals.h"
#include "utils.h"
#include "sprites.h"

void createEntity(unsigned char tile, unsigned char entityId, unsigned short x, unsigned short y) {
    if (tile == TILE_GENERATOR) {
        entityList[entityId].isGenerator = 1;
        entityList[entityId].health =  60;
        entityList[entityId].spawnRate = 60;
        entityList[entityId].nextSpawn = entityList[entityId].spawnRate;
    } else {
        entityList[entityId].isGenerator = 0;
        entityList[entityId].health =  60;
        entityList[entityId].hasTarget = 0;
        entityList[entityId].animationCount = ANIMATION_FRAME_SPEED;
        entityList[entityId].animationFrame = 0;
        entityList[entityId].facingX = 0;
    }

    entityList[entityId].spriteId = entityId+2; // First 2 sprites are guy and weapon
    entityList[entityId].x = x * 16;
    entityList[entityId].y = y * 16;
    entityList[entityId].visible = 0;
    entityList[entityId].currentTileX = x;
    entityList[entityId].currentTileY = y;
}

void createMapStatus() {
    unsigned char x,y,i;
    Entity *lastEntity = 0;

    loadFileToBankedRAM("l0map.bin", MAP_BANK, 0);
    BANK_NUM = MAP_BANK;

    entitySleepList = 0;

    for (y=0,i=0; y<MAP_MAX; y++) {
        for (x=0; x<MAP_MAX; x++) {
            if (mapStatus[y][x] == TILE_ENTITY || mapStatus[y][x] == TILE_GENERATOR) {
                createEntity(mapStatus[y][x], i, x, y);

                entityList[i].prev = lastEntity; // For the LL, prev entity
                entityList[i].next = 0;

                // If we have a lastEntity (not at beginning of list)
                // Point it's 'next' to this entity
                if (lastEntity) {
                    lastEntity->next = &entityList[i];
                }

                // Move the lastEntity to be this new entity (for the next iteration)
                lastEntity = &entityList[i];

                mapStatus[y][x] = ENTITY_TILE_START + entityList[i].spriteId;
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
    loadFileToVRAM("l0tile.bin", L0_MAPBASE_ADDR);
}