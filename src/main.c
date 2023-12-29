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

unsigned char weaponRotation[4] = {0, 1, 3, 2};
short scrollX, scrollY;

void setGuyDirection() {
    unsigned char joy;

    joy = joy_read(0);

    guy.pressedX = 0;
    guy.pressedY = 0;
    guy.pressedShoot = 0;

    if (JOY_LEFT(joy)) {
        guy.pressedX = -1;
    } else if (JOY_RIGHT(joy)) {
        guy.pressedX = 1;
    }

    if (JOY_UP(joy)) {
        guy.pressedY = -1;
    } else if (JOY_DOWN(joy)) {
        guy.pressedY = 1;
    }

    if (JOY_BTN_1(joy) || JOY_BTN_2(joy)) {
        guy.pressedShoot = 1;
    }
}

unsigned char tryTile(unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY) {
    unsigned char tile = mapStatus[toY][toX];
    signed char i;

    if (tile == TILE_KEY) {
        guy.keys += 1;
        mapStatus[toY][toX] = TILE_FLOOR;
        copyTile(fromX, fromY, toX, toY);
    } if (tile == TILE_CHEST) {
        guy.gold += 500;
        guy.score += 500;
        mapStatus[toY][toX] = TILE_FLOOR;
        copyTile(fromX, fromY, toX, toY);
    } else if (tile == TILE_DOOR && guy.keys > 0) {
        guy.keys -= 1;
        mapStatus[toY][toX] = TILE_FLOOR;
        copyTile(fromX, fromY, toX, toY);

        i=1;
        while(mapStatus[toY-i][toX] == TILE_DOOR) {
            mapStatus[toY-i][toX] = TILE_FLOOR;
            copyTile(fromX, fromY, toX, toY-i);
            i++;
        }

        i=1;
        while(mapStatus[toY+i][toX] == TILE_DOOR) {
            mapStatus[toY+i][toX] = TILE_FLOOR;
            copyTile(fromX, fromY, toX, toY+i);
            i++;
        }

        i=1;
        while(mapStatus[toY][toX-i] == TILE_DOOR) {
            mapStatus[toY][toX-i] = TILE_FLOOR;
            copyTile(fromX, fromY, toX-i, toY);
            i++;
        }

        i=1;
        while(mapStatus[toY][toX+i] == TILE_DOOR) {
            mapStatus[toY][toX+i] = TILE_FLOOR;
            copyTile(fromX, fromY, toX+i, toY);
            i++;
        }
        
    }

    if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
        return 1;
    }

    return 0;
}

void moveGuy(unsigned char speed) {
    unsigned short prevX, prevY, shot = 0;
    unsigned char tile, tempTileX, tempTileY;
    signed char dirX = 0, dirY = 0;
    Entity *entity;

    guy.currentTileX = (guy.x+8)>>4;
    guy.currentTileY = (guy.y+8)>>4;

    // Pause
    // if (JOY_BTN_1(joy)) {
    //     waitForRelease();
    //     waitForButtonPress();
    // }
    prevX = guy.x;
    prevY = guy.y;

    if (guy.pressedX == -1) {
        dirX = -1;
        guy.x -= speed;
        guy.facingX = 1;
    } else if (guy.pressedX == 1) {
        dirX = 1;
        guy.x += speed;
        guy.facingX = 0;
    }

    if (dirX == 1) {
        // Check if new tile is open...move back if not
        if (tryTile(guy.currentTileX, guy.currentTileY, (guy.x+15)>>4, guy.y>>4)) {
            dirX = 0;
            guy.x = prevX;
        } else {
            if (tryTile(guy.currentTileX, guy.currentTileY, (guy.x+15)>>4, (guy.y+15)>>4)) {
                dirX = 0;
                guy.x = prevX;
            }
        }
    } else if (dirX == -1) {
        // Check if new tile is open...move back if not
        if (tryTile(guy.currentTileX, guy.currentTileY, guy.x>>4, guy.y>>4)) {
            dirX = 0;
            guy.x = prevX;
        } else {
            if (tryTile(guy.currentTileX, guy.currentTileY, guy.x>>4, (guy.y+15)>>4)) {
                dirX = 0;
                guy.x = prevX;
            }
        }
    }

    if (guy.pressedY == -1) {
        dirY = -1;
        guy.y -= speed;
    } else if (guy.pressedY == 1) {
        dirY = 1;
        guy.y += speed;
    }

    if (dirY == 1) {
        // Check if new tile is open...move back if not
        if (tryTile(guy.currentTileX, guy.currentTileY, (guy.x+15)>>4, (guy.y+15)>>4)) {
            dirY = 0;
            guy.y = prevY;
        } else {
            if (tryTile(guy.currentTileX, guy.currentTileY, guy.x>>4, (guy.y+15)>>4)) {
                dirY = 0;
                guy.y = prevY;
            }
        }
    } else if (dirY == -1) {
        // Check if new tile is open...move back if not
        if (tryTile(guy.currentTileX, guy.currentTileY, guy.x>>4, guy.y>>4)) {
            dirY = 0;
            guy.y = prevY;
        } else {
            if (tryTile(guy.currentTileX, guy.currentTileY, (guy.x+15)>>4, guy.y>>4)) {
                dirY = 0;
                guy.y = prevY;
            }
        }
    }

    // Set the guy's aiming direction
    if (guy.pressedX != 0 && guy.pressedY == 0) {
        guy.aimX = guy.pressedX;
        guy.aimY = 0;
    } else if (guy.pressedX == 0 && guy.pressedY != 0) {
        guy.aimX = 0;
        guy.aimY = guy.pressedY;
    } else if (guy.pressedX != 0 && guy.pressedY != 0) {
        guy.aimX = guy.pressedX;
        guy.aimY = guy.pressedY;
    } // Other cases just leave the facing as it is


    // Check if attacking
    tile = mapStatus[((guy.y+8)+(dirY*8))>>4][((guy.x+8)+(dirX*8))>>4];
    if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
        entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
        if (entity) {
            attackEntity(entity, MELEE_DAMAGE);
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

    if (guy.pressedShoot && !weapon.visible && guy.ticksUntilNextShot == 0) {
        guy.ticksUntilNextShot = GUY_SHOOT_TICKS;
        weapon.x = guy.x;
        weapon.y = guy.y;
        weapon.visible = 1;
        weapon.dirX = guy.aimX;
        weapon.dirY = guy.aimY;
        weapon.animationCount = WEAPON_ROTATION_SPEED;
        weapon.animationFrame = 0;

        toggleWeapon(1);

        shot = 1;
    } else {
        if (guy.ticksUntilNextShot > 0) {
            guy.ticksUntilNextShot -= 1;
        }
    }

    // Animate the guy
    if (shot || guy.x != prevX || guy.y != prevY || guy.animationFrame == ANIMATION_FRAME_COUNT) {
        if (shot) {
            guy.animationChange = 1;
            guy.animationCount = ANIMATION_FRAME_SPEED;
            guy.animationFrame = ANIMATION_FRAME_COUNT;
        } 
        
        if (guy.animationCount == 0) {
            guy.animationChange = 1;
            guy.animationCount = ANIMATION_FRAME_SPEED;
            if (guy.animationFrame >= ANIMATION_FRAME_COUNT-1) {
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
    guy.health = 1000;
    guy.animationCount = ANIMATION_FRAME_SPEED;
    guy.animationFrame = 0;
    guy.ticksUntilNextMelee = 0;
    guy.ticksUntilNextShot = 0;
    guy.shooting = 0;
    guy.aimX = 1;
    guy.aimY = 0;
    guy.facingX = 0;
    guy.score = 0;
    guy.gold = 0;
    guy.keys = 0;
}

void moveWeapon() {
    unsigned char tile;
    Entity *entity;

    if (weapon.visible) {
        weapon.animationCount -= 1;
        if (weapon.animationCount == 0) {
            weapon.animationCount = WEAPON_ROTATION_SPEED;
            weapon.animationFrame += 1;
            if (weapon.animationFrame == 4) {
                weapon.animationFrame = 0;
            }
        }

        // We check before and after the move because of tile boundary edge cases
        // Probably a better way but tile checking is really fast...so, maybe this is ok
        tile = mapStatus[(weapon.y+8)>>4][(weapon.x+8)>>4];
        if (tile != TILE_FLOOR && tile != GUY_CLAIM && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapon.visible = 0;
            toggleWeapon(0);

            if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    attackEntity(entity, WEAPON_DAMAGE);
                }
            }
            return;
        }

        weapon.x += weapon.dirX * WEAPON_SPEED;
        weapon.y += weapon.dirY * WEAPON_SPEED;

        // See if weapon is off screen
        if (weapon.x >= scrollX + SCROLL_PIXEL_SIZE || weapon.x+16 <= scrollX || weapon.y >= scrollY + SCROLL_PIXEL_SIZE || weapon.y+16 <= scrollY) {
            // Hide it for now
            weapon.visible = 0;
            toggleWeapon(0);
            return;
        }

        moveAndSetAnimationFrame(1, weapon.x, weapon.y, scrollX, scrollY, AXE_TILE, 0, weaponRotation[weapon.animationFrame]);

        // Check if hit something after the move
        tile = mapStatus[(weapon.y+8)>>4][(weapon.x+8)>>4];
        if (tile != TILE_FLOOR && tile != GUY_CLAIM && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapon.visible = 0;
            toggleWeapon(0);

            if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    attackEntity(entity, WEAPON_DAMAGE);
                }
            }
        }
    }
}

void main() {
    unsigned char count = 0, load;
    unsigned char inputTicks = 0;
    Entity *entity;

    init();
    initTiles();
    spritesConfig();
    clearLayers();
    drawOverlayBackground();
    createMapStatus();
    drawMap();

    setupGuy();

    while(1) {
        // Get joystick input only periodically
        if (inputTicks == 4) {
            setGuyDirection();
            inputTicks = 0;
        } else {
            inputTicks++;
        }

        moveGuy(count == 0 ? GUY_SPEED_1 : GUY_SPEED_2);

        scrollX = guy.x-112;
        if (scrollX < 0) {
            scrollX = 0;
        } else if (scrollX > maxMapX) {
            scrollX = maxMapX;
        }

        scrollY = guy.y-112;
        if (scrollY < 0) {
            scrollY = 0;
        } else if (scrollY > maxMapY) {
            scrollY = maxMapY;
        }
        
        VERA.layer0.vscroll = scrollY;
        VERA.layer0.hscroll = scrollX;

        // Only set his animation frame if needed (this is more expensive)
        // Otherwise just move him
        if (guy.animationChange) {
            guy.animationChange = 0;
            moveAndSetAnimationFrame(0, guy.x, guy.y, scrollX, scrollY, GUY_TILE, guy.animationFrame, guy.facingX);
        } else {
            moveSpriteId(0, guy.x, guy.y, scrollX, scrollY);
        }

        moveWeapon();

        if (count == 0) {
            // activation/deactivation phase
            activateEntities(scrollX, scrollY);
            deactivateEntities(scrollX, scrollY);
            tempActiveToActiveEntities();

            // Move "some" active entities
            // Try to split the load a bit
            entity = entityActiveList;
            load=0;
            while(entity && load<10) {
                moveEntity(entity, guy.currentTileX, guy.currentTileY, scrollX, scrollY);   
                entity->movedPrevTick=1;
                entity = entity->next;
                load++;
            };
        } else {
            // Move active entities phase
            entity = entityActiveList;

            while(entity) {
                if (!entity->movedPrevTick) {
                    moveEntity(entity, guy.currentTileX, guy.currentTileY, scrollX, scrollY);   
                } else {
                    entity->movedPrevTick = 0;
                }
                entity = entity->next;
            };

            updateOverlay();
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