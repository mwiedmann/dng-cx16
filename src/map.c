#include <cx16.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "tiles.h"
#include "map.h"
#include "globals.h"
#include "utils.h"
#include "sprites.h"

#pragma code-name (push, "BANKRAM02")

void createEntityFill(Entity *entity, unsigned char tile, unsigned char entityId, unsigned char xTile, unsigned char yTile, unsigned char isShot) {
    unsigned long addr;

    if (tile >= TILE_GENERATOR_START && tile <= TILE_GENERATOR_END) {
        entity->isGenerator = 1;
        entity->entityTypeId = tile-TILE_GENERATOR_START;
        entity->tileId = GENERATOR_TILE+entity->entityTypeId;
        entity->health =  12;
        entity->animationCount = 0;
        entity->animationFrame = 0;
        entity->spawnRate = level < 5 ? 60 : level < 10 ? 55 : level < 15 ? 50 : level < 25 ? 45 : 40;
        entity->nextSpawn = entity->spawnRate;
        entity->points = 1000;
        entity->stats = 0; // Generators don't have monster stats
    } else {
        entity->isGenerator = 0;
        entity->entityTypeId = tile-TILE_ENTITY_START;
        entity->stats = entityStatsByType[entity->entityTypeId];
        entity->health =  entity->stats->startingHealth;
        entity->tileId = isShot
            ? MONSTER_PROJECTILE_TILE + (entity->entityTypeId>>1)
            : MONSTER_TILE+(4*entity->entityTypeId);
        entity->hasTarget = 0;
        entity->animationCount = ANIMATION_FRAME_SPEED;
        entity->animationFrame = 0;
        entity->facingX = 0;
        entity->points = entity->stats->points;
    }

    entity->statsId = 0;
    entity->spriteId = entityId+4; // First 4 sprites are guy and weapon

    addr = SPRITE_ADDR_START + (entity->spriteId * 8);
    
    // The sprite address is fixed...save it to avoid recalcing every time
    entity->spriteAddrLo = addr;
    entity->spriteAddrHi = addr>>16;

    // Calculate the tile graphic address
    // Animations will change the frame (and the address) a tad and we can add that part later (still faster)
    // NOTE: Its possible that a frame increment could push this address beyond 64k and the "Hi" part would then be wrong
    // Let's deal with that by avoiding having animation frame cross that boundary!
    addr = L0_TILEBASE_ADDR + (entity->tileId*L0_TILE_SIZE);
    entity->spriteGraphicLo = addr>>5;
    entity->spriteGraphicHi = addr>>13;
    
    entity->x = xTile * 16;
    entity->y = yTile * 16;
    entity->visible = 0;
    entity->currentTileX = xTile;
    entity->currentTileY = yTile;

    entity->animationChange = 1;
    entity->movedPrevTick = 0;
    entity->rangedTicks = entity->stats->rangedRate;
    entity->isShot = isShot;
    entity->isLob = 0;
    entity->wasHit = 0;
}


#pragma code-name (pop)

void createEntity(unsigned char tile, unsigned char entityId, unsigned char xTile, unsigned char yTile, unsigned char isShot) {
    Entity entity;

    RAM_BANK = CODE_BANK;
    createEntityFill(&entity, tile, entityId, xTile, yTile, isShot);
    RAM_BANK = MAP_BANK;

    memcpy(&entityList[entityId], &entity, sizeof(Entity));
}

#pragma code-name (push, "BANKRAM02")

void drawMap(unsigned char level) {
    char buf[16];

    sprintf(buf, "l%utile.bin", level);
    loadFileToVRAM(buf, L0_MAPBASE_ADDR);
}

#pragma code-name (pop)

void createMapStatus(unsigned char level) {
    char buf[16];
    unsigned char x,y,i;
    Entity *lastEntity = 0;

    sprintf(buf, "l%umap.bin", level);
    // mapStatus starts in Banked RAM AFTER some code
    // Use MAP_BANK_CODE_SIZE to skip that part of ram
    loadFileToBankedRAM(buf, MAP_BANK, MAP_BANK_CODE_SIZE);

    RAM_BANK = MAP_BANK;

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
                createEntity(mapStatus[y][x], i, x, y, 0);

                entityList[i].prev = lastEntity; // For the LL, prev entity
                entityList[i].next = 0;

                // If we have a lastEntity (not at beginning of list)
                // Point it's 'next' to this entity
                if (lastEntity) {
                    lastEntity->next = &entityList[i];
                }

                // Move the lastEntity to be this new entity (for the next iteration)
                lastEntity = &entityList[i];

                mapStatus[y][x] = ENTITY_TILE_START + (entityList[i].spriteId-4);
                i++;
            } else if (mapStatus[y][x] == TILE_GUY /* Guy Start*/) {
                mapStatus[y][x] = 0;
                players[0].x = (x * 16)+1;
                players[0].y = (y * 16)+1;

                mapStatus[y][x+1] = 0;
                players[1].x = ((x+1) * 16)+1; // Start next to each other
                players[1].y = (y * 16)+1;
            }
        }
    }

    maxMapX -= SCROLL_PIXEL_SIZE-16;
    maxMapY -= SCROLL_PIXEL_SIZE-16;

    // This will put all the entities in the sleeping list (if there are any)
    if (entityList[0].health > 0) {
        entitySleepList = &entityList[0];
    }
}
