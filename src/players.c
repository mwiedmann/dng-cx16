#include <cx16.h>
#include <joystick.h>

#include "globals.h"
#include "tiles.h"
#include "sprites.h"
#include "list.h"
#include "ai.h"
#include "utils.h"

void setGuyDirection(unsigned char playerId) {
    unsigned char joy;

    joy = joy_read(0);

    players[playerId].pressedX = 0;
    players[playerId].pressedY = 0;
    players[playerId].pressedShoot = 0;

    // Button 3 controls which player is moving. A hack for now since I don't have a joypad (that works!)
    if (playerId == 0 && JOY_BTN_3(joy) || (playerId == 1 && !JOY_BTN_3(joy))) {
        return;
    }

    if (JOY_LEFT(joy)) {
        players[playerId].pressedX = -1;
    } else if (JOY_RIGHT(joy)) {
        players[playerId].pressedX = 1;
    }

    if (JOY_UP(joy)) {
        players[playerId].pressedY = -1;
    } else if (JOY_DOWN(joy)) {
        players[playerId].pressedY = 1;
    }

    if (JOY_BTN_1(joy)) {
        players[playerId].pressedShoot = 1;
    }

    if (JOY_BTN_2(joy)) {
        if (players[playerId].pressedScroll == 0) {
            players[playerId].pressedScroll = 1;
        }
    } else {
        players[playerId].pressedScroll = 0;
    }
}

unsigned char tryTile(unsigned char playerId, unsigned char fromTileX, unsigned char fromTileY, unsigned short toX, unsigned short toY) {
    unsigned char toTileX = toX>>4;
    unsigned char toTileY = toY>>4;
    unsigned char tile = mapStatus[toTileY][toTileX];
    signed char i;

    if (tile == TILE_KEY) {
        if (!hints.keys) {
            hints.keys = 1;
            gameMessage(8, 15, "COLLECT KEYS", 8, 16, "TO OPEN DOORS");
        }

        players[playerId].keys += 1;
        players[playerId].score += 100;
        overlayChanged = 1;
        mapStatus[toTileY][toTileX] = TILE_FLOOR;
        copyTile(fromTileX, fromTileY, toTileX, toTileY);
    } if (tile == TILE_TREASURE_CHEST || tile == TILE_TREASURE_GOLD || tile == TILE_TREASURE_SILVER) {
        players[playerId].gold += tile == TILE_TREASURE_CHEST ? 500 : tile == TILE_TREASURE_GOLD ? 250 : 100;
        players[playerId].score += tile == TILE_TREASURE_CHEST ? 500 : tile == TILE_TREASURE_GOLD ? 250 : 100;
        overlayChanged = 1;
        mapStatus[toTileY][toTileX] = TILE_FLOOR;
        copyTile(fromTileX, fromTileY, toTileX, toTileY);
    } else if (tile == TILE_SCROLL) {
        players[playerId].scrolls += 1;
        players[playerId].score += 250;
        overlayChanged = 1;
        mapStatus[toTileY][toTileX] = TILE_FLOOR;
        copyTile(fromTileX, fromTileY, toTileX, toTileY);
        return 0;
    } else if (tile == TILE_FOOD_BIG || tile == TILE_FOOD_SMALL) {
        players[playerId].health += tile == TILE_FOOD_BIG ? players[playerId].stats->foodHealthBig : players[playerId].stats->foodHealthSmall;
        players[playerId].score += tile == TILE_FOOD_BIG ? 250 : 100;
        overlayChanged = 1;
        mapStatus[toTileY][toTileX] = TILE_FLOOR;
        copyTile(fromTileX, fromTileY, toTileX, toTileY);
        return 0;
    } else if (tile == TILE_DOOR && players[playerId].keys > 0) {
        players[playerId].keys -= 1;
        overlayChanged = 1;
        mapStatus[toTileY][toTileX] = TILE_FLOOR;
        copyTile(fromTileX, fromTileY, toTileX, toTileY);

        i=1;
        while(mapStatus[TILE_ADJ(toTileY-i)][toTileX] == TILE_DOOR) {
            mapStatus[TILE_ADJ(toTileY-i)][toTileX] = TILE_FLOOR;
            copyTile(fromTileX, fromTileY, toTileX, TILE_ADJ(toTileY-i));
            i++;
        }

        i=1;
        while(mapStatus[TILE_ADJ(toTileY+i)][toTileX] == TILE_DOOR) {
            mapStatus[TILE_ADJ(toTileY+i)][toTileX] = TILE_FLOOR;
            copyTile(fromTileX, fromTileY, toTileX, TILE_ADJ(toTileY+i));
            i++;
        }

        i=1;
        while(mapStatus[toTileY][TILE_ADJ(toTileX-i)] == TILE_DOOR) {
            mapStatus[toTileY][TILE_ADJ(toTileX-i)] = TILE_FLOOR;
            copyTile(fromTileX, fromTileY, TILE_ADJ(toTileX-i), toTileY);
            i++;
        }

        i=1;
        while(mapStatus[toTileY][TILE_ADJ(toTileX+i)] == TILE_DOOR) {
            mapStatus[toTileY][TILE_ADJ(toTileX+i)] = TILE_FLOOR;
            copyTile(fromTileX, fromTileY, TILE_ADJ(toTileX+i), toTileY);
            i++;
        }
        
    } else if (tile == TILE_EXIT) {
        players[playerId].exit = 1;
        return 0;
    }

    if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
        return 1;
    }

    // Scrolling is split between both players in 2 player game
    // Make sure players aren't leaving the scroll field
    if (activePlayers == 2) {
        // TODO: Need wrapping logic
        if (toX >= scrollX + TWO_PLAYER_SCROLL_LIMIT || toX <= scrollX+16 || toY >= scrollY + TWO_PLAYER_SCROLL_LIMIT || toY <= scrollY+16) {
            return 1;
        }
    }

    return 0;
}

void moveGuy(unsigned char playerId, unsigned char speed) {
    unsigned short prevX, prevY, shot = 0;
    unsigned char tile, tempTileX, tempTileY;
    signed char dirX = 0, dirY = 0;
    Entity *entity;

    players[playerId].currentTileX = TILE_ADJ((players[playerId].x+8)>>4);
    players[playerId].currentTileY = TILE_ADJ((players[playerId].y+8)>>4);

    // Pause
    // if (JOY_BTN_1(joy)) {
    //     waitForRelease();
    //     waitForButtonPress();
    // }
    prevX = players[playerId].x;
    prevY = players[playerId].y;

    if (players[playerId].pressedX == -1) {
        if (!players[playerId].pressedShoot) {
            dirX = -1;
            players[playerId].x = POS_ADJ(players[playerId].x - speed);
        }
        players[playerId].facingX = 1;
    } else if (players[playerId].pressedX == 1) {
        if (!players[playerId].pressedShoot) {
            dirX = 1;
            players[playerId].x = POS_ADJ(players[playerId].x + speed);
        }
        players[playerId].facingX = 0;
    }

    // For this section we will test the corners of the player sprite to make sure its not touching solid tiles
    // This is to prevent it looking like the player is clipping through tiles

    if (dirX == 1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, POS_ADJ(players[playerId].x+15), players[playerId].y)) {
            dirX = 0;
            players[playerId].x = prevX;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, POS_ADJ(players[playerId].x+15), POS_ADJ(players[playerId].y+15))) {
                dirX = 0;
                players[playerId].x = prevX;
            }
        }
    } else if (dirX == -1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x, players[playerId].y)) {
            dirX = 0;
            players[playerId].x = prevX;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x, POS_ADJ(players[playerId].y+15))) {
                dirX = 0;
                players[playerId].x = prevX;
            }
        }
    }

    if (!players[playerId].pressedShoot) {
        if (players[playerId].pressedY == -1) {
            dirY = -1;
            players[playerId].y = POS_ADJ(players[playerId].y - speed);
        } else if (players[playerId].pressedY == 1) {
            dirY = 1;
            players[playerId].y = POS_ADJ(players[playerId].y + speed);
        }
    }

    if (dirY == 1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, POS_ADJ(players[playerId].x+15), POS_ADJ(players[playerId].y+15))) {
            dirY = 0;
            players[playerId].y = prevY;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x, POS_ADJ(players[playerId].y+15))) {
                dirY = 0;
                players[playerId].y = prevY;
            }
        }
    } else if (dirY == -1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x, players[playerId].y)) {
            dirY = 0;
            players[playerId].y = prevY;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, POS_ADJ(players[playerId].x+15), players[playerId].y)) {
                dirY = 0;
                players[playerId].y = prevY;
            }
        }
    }

    // Set the guy's aiming direction
    if (players[playerId].pressedX != 0 && players[playerId].pressedY == 0) {
        players[playerId].aimX = players[playerId].pressedX;
        players[playerId].aimY = 0;
    } else if (players[playerId].pressedX == 0 && players[playerId].pressedY != 0) {
        players[playerId].aimX = 0;
        players[playerId].aimY = players[playerId].pressedY;
    } else if (players[playerId].pressedX != 0 && players[playerId].pressedY != 0) {
        players[playerId].aimX = players[playerId].pressedX;
        players[playerId].aimY = players[playerId].pressedY;
    } // Other cases just leave the facing as it is


    // Check if melee attacking
    if (!players[playerId].pressedShoot) {
        tile = mapStatus[POS_ADJ(((players[playerId].y+8)+(dirY*8)))>>4][POS_ADJ(((players[playerId].x+8)+(dirX*8)))>>4];
        if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
            if (players[playerId].ticksUntilNextMelee == 0) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    players[playerId].ticksUntilNextMelee = players[playerId].stats->ticksToMelee + 1;
                    attackEntity(playerId, entity, players[playerId].stats->meleeDamage);
                    if (entity->health > 0) {
                        // Entity not dead yet...guy doesn't move
                        players[playerId].x = prevX;
                        players[playerId].y = prevY;
                    }
                }
            } else {
                // Can't attack yet...move back
                players[playerId].x = prevX;
                players[playerId].y = prevY;
            }
        }
    }
    
    if (players[playerId].ticksUntilNextMelee > 0) {
        players[playerId].ticksUntilNextMelee -= 1;
    }

    tempTileX = POS_ADJ(players[playerId].x+8)>>4;
    tempTileY = POS_ADJ(players[playerId].y+8)>>4;

    // See if guy has moved tiles
    if (players[playerId].currentTileX != tempTileX || players[playerId].currentTileY != tempTileY) {
        // Open up the previous tile
        mapStatus[players[playerId].currentTileY][players[playerId].currentTileX] = TILE_FLOOR;

        players[playerId].currentTileX = tempTileX;
        players[playerId].currentTileY = tempTileY;
    }

    // Stamp the current tile with the guy
    mapStatus[players[playerId].currentTileY][players[playerId].currentTileX] = GUY_CLAIM+playerId;

    if (players[playerId].pressedShoot && !weapons[playerId].visible && players[playerId].ticksUntilNextShot == 0) {
        players[playerId].ticksUntilNextShot = players[playerId].stats->ticksToRanged;
        weapons[playerId].x = players[playerId].x;
        weapons[playerId].y = players[playerId].y;
        weapons[playerId].visible = 1;
        weapons[playerId].dirX = players[playerId].aimX;
        weapons[playerId].dirY = players[playerId].aimY;
        weapons[playerId].animationCount = WEAPON_ROTATION_SPEED;
        weapons[playerId].animationFrame = 0;

        toggleWeapon(playerId, 1);

        shot = 1;
    } else {
        if (players[playerId].ticksUntilNextShot > 0) {
            players[playerId].ticksUntilNextShot -= 1;
        }
    }

    // See if player used scroll
    if (players[playerId].pressedScroll == 1 && players[playerId].scrolls > 0) {
        players[playerId].pressedScroll = 2;
        players[playerId].scrolls -= 1;
        useScrollOnEntities(playerId);
    }

    // Animate the guy
    if (shot || players[playerId].x != prevX || players[playerId].y != prevY || players[playerId].animationFrame == ANIMATION_FRAME_COUNT) {
        if (shot) {
            players[playerId].animationChange = 1;
            players[playerId].animationCount = ANIMATION_FRAME_SPEED;
            players[playerId].animationFrame = ANIMATION_FRAME_COUNT;
        } 
        
        if (players[playerId].animationCount == 0) {
            players[playerId].animationChange = 1;
            players[playerId].animationCount = ANIMATION_FRAME_SPEED;
            if (players[playerId].animationFrame >= ANIMATION_FRAME_COUNT-1) {
                players[playerId].animationFrame = 0;
            } else {
                players[playerId].animationFrame += 1;
            }
        } else {
            players[playerId].animationCount -= 1;
        }
    }
}

void setupPlayer(unsigned char playerId, enum Character characterType) {
    players[playerId].active = 1;
    players[playerId].characterType = characterType;
    players[playerId].stats =  playerStatsByType[characterType];
    players[playerId].health = players[playerId].stats->startingHealth;
    players[playerId].animationCount = ANIMATION_FRAME_SPEED;
    players[playerId].animationFrame = 0;
    players[playerId].animationTile = GUY_TILE_START + (playerId*32) + (characterType*8);
    players[playerId].weaponTile = GUY_TILE_START + (playerId*32) + (characterType*8) + 5;
    players[playerId].ticksUntilNextMelee = 0;
    players[playerId].ticksUntilNextShot = 0;
    players[playerId].shooting = 0;
    players[playerId].aimX = 1;
    players[playerId].aimY = 0;
    players[playerId].facingX = 0;
    players[playerId].score = 0;
    players[playerId].gold = 0;
    players[playerId].keys = 0;
    players[playerId].scrolls = 0;
    players[playerId].exit = 0;
    players[playerId].animationChange = 1; // Trigger immediate animation

    overlayChanged = 1;
}

void moveWeapon(unsigned char playerId) {
    unsigned char tile;
    Entity *entity;

    if (weapons[playerId].visible) {
        weapons[playerId].animationCount -= 1;
        if (weapons[playerId].animationCount == 0) {
            weapons[playerId].animationCount = WEAPON_ROTATION_SPEED;
            weapons[playerId].animationFrame += 1;
            if (weapons[playerId].animationFrame == 4) {
                weapons[playerId].animationFrame = 0;
            }
        }

        // We check before and after the move because of tile boundary edge cases
        // Probably a better way but tile checking is really fast...so, maybe this is ok
        tile = mapStatus[POS_ADJ(weapons[playerId].y+8)>>4][POS_ADJ(weapons[playerId].x+8)>>4];
        if (tile != TILE_FLOOR && tile < GUY_CLAIM && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleWeapon(playerId, 0);

            if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    attackEntity(playerId, entity, players[playerId].stats->rangedDamage);
                }
            }
            return;
        }

        weapons[playerId].x = POS_ADJ(weapons[playerId].x + (weapons[playerId].dirX * WEAPON_SPEED));
        weapons[playerId].y = POS_ADJ(weapons[playerId].y + (weapons[playerId].dirY * WEAPON_SPEED));

        // See if weapon is off screen
        if (offScreen(weapons[playerId].x, weapons[playerId].y)) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleWeapon(playerId, 0);
            return;
        }

        moveAndSetAnimationFrame(WEAPON_SPRITE_ID_START+playerId, weapons[playerId].x, weapons[playerId].y, players[playerId].weaponTile, 0, weaponRotation[weapons[playerId].animationFrame]);

        // Check if hit something after the move
        tile = mapStatus[POS_ADJ(weapons[playerId].y+8)>>4][POS_ADJ(weapons[playerId].x+8)>>4];
        if (tile != TILE_FLOOR && tile < GUY_CLAIM && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleWeapon(playerId, 0);

            if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    attackEntity(playerId, entity, players[playerId].stats->rangedDamage);
                }
            }
        }
    }
}
