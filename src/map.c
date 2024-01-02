#include <cx16.h>
#include <stdio.h>

#include "config.h"
#include "tiles.h"
#include "map.h"
#include "globals.h"
#include "utils.h"
#include "sprites.h"

void createEntity(unsigned char tile, unsigned char entityId, unsigned short x, unsigned short y) {
    if (tile >= TILE_GENERATOR_START && tile <= TILE_GENERATOR_END) {
        entityList[entityId].isGenerator = 1;
        entityList[entityId].entityTypeId = tile-TILE_GENERATOR_START;
        entityList[entityId].tileId = GENERATOR_TILE+entityList[entityId].entityTypeId;
        entityList[entityId].health =  12;
        entityList[entityId].animationCount = 0;
        entityList[entityId].animationFrame = 0;
        entityList[entityId].spawnRate = 60;
        entityList[entityId].nextSpawn = entityList[entityId].spawnRate;
        entityList[entityId].points = 1000;
    } else {
        entityList[entityId].isGenerator = 0;
        entityList[entityId].entityTypeId = tile-TILE_ENTITY_START;
        entityList[entityId].stats = entityStatsByType[entityList[entityId].entityTypeId];
        entityList[entityId].health =  entityList[entityId].stats->startingHealth;
        entityList[entityId].tileId = MONSTER_TILE+(4*entityList[entityId].entityTypeId);
        entityList[entityId].hasTarget = 0;
        entityList[entityId].animationCount = ANIMATION_FRAME_SPEED;
        entityList[entityId].animationFrame = 0;
        entityList[entityId].facingX = 0;
        entityList[entityId].points = 50; // TODO: Vary by entity type
    }

    entityList[entityId].statsId = 0;
    entityList[entityId].spriteId = entityId+4; // First 4 sprites are guy and weapon
    entityList[entityId].x = x * 16;
    entityList[entityId].y = y * 16;
    entityList[entityId].visible = 0;
    entityList[entityId].currentTileX = x;
    entityList[entityId].currentTileY = y;
    entityList[entityId].animationChange = 1;
    entityList[entityId].movedPrevTick = 0;
}

void createMapStatus(unsigned char level) {
    char buf[16];
    unsigned char x,y,i;
    Entity *lastEntity = 0;

    sprintf(buf, "l%umap.bin", level);
    loadFileToBankedRAM(buf, MAP_BANK, 0);

    BANK_NUM = MAP_BANK;

    entitySleepList = 0;
    
    maxMapX = 0;
    maxMapY = 0;

    for (y=0,i=0; y<MAP_MAX; y++) {
        for (x=0; x<MAP_MAX; x++) {
            if (mapStatus[y][x] != 0) {
                if (x*16 > maxMapX) {
                    maxMapX = x*16;
                }

                if (y*16 > maxMapY) {
                    maxMapY = y*16;
                }
            }
            if (mapStatus[y][x] >= TILE_GENERATOR_START && mapStatus[y][x] <= TILE_ENTITY_END) {
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
                players[0].x = x * 16;
                players[0].y = y * 16;

                players[1].x = (x+1) * 16; // Start next to each other
                players[1].y = y * 16;
            }
        }
    }

    maxMapX -= SCROLL_PIXEL_SIZE-16;
    maxMapY -= SCROLL_PIXEL_SIZE-16;

    // This will put all the entities in the sleeping list
    entitySleepList = &entityList[0];
}

void drawMap(unsigned char level) {
    char buf[16];

    sprintf(buf, "l%utile.bin", level);
    loadFileToVRAM(buf, L0_MAPBASE_ADDR);
}