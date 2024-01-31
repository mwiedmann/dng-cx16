#include <cx16.h>
#include <joystick.h>
#include <stdio.h>

#include "globals.h"
#include "tiles.h"
#include "sprites.h"
#include "list.h"
#include "ai.h"
#include "utils.h"
#include "config.h"
#include "sound.h"
#include "strtbl.h"

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

void stopMove(unsigned char playerId) {
    players[playerId].pressedX = 0;
    players[playerId].pressedY = 0;
}

void cannotAfford(unsigned char playerId) {
    RAM_BANK = CODE_BANK;
    gameMessage(STRING_GAME_MSG_CANNOT_AFFORD, 0);
    RAM_BANK = MAP_BANK;
    stopMove(playerId);
}

unsigned char tryTile(unsigned char playerId, unsigned char fromX, unsigned char fromY, unsigned short toX, unsigned short toY) {
    unsigned char toTileX = toX>>4;
    unsigned char toTileY = toY>>4;
    unsigned char tile = mapStatus[toTileY][toTileX];
    signed char i;
    unsigned char clearTile=0, id;

    // Scrolling is split between both players in 2 player game
    // Make sure players aren't leaving the scroll field
    if (activePlayers == 2) {
        if (toX >= scrollX + TWO_PLAYER_SCROLL_LIMIT || toX <= scrollX+16 || toY >= scrollY + TWO_PLAYER_SCROLL_LIMIT || toY <= scrollY+16) {
            return 1;
        }
    }

    // Most common case...nothing else to check
    if (tile == TILE_FLOOR) {
        return 0;
    }

    // There is an inventory limit on keys+scrolls
    if (tile == TILE_KEY && (players[playerId].keys + players[playerId].scrolls < INVENTORY_LIMIT)) {
        if (!hints.keys) {
            hints.keys = 1;
            RAM_BANK = CODE_BANK;
            gameMessage(STRING_GAME_MSG_COLLECT_KEYS, SOUND_MUSIC_KEYS);
            RAM_BANK = MAP_BANK;
        }
        
        if (isShopLevel) {
            if (players[playerId].gold < KEY_PRICE) {
                cannotAfford(playerId);
                return 1;
            }

            RAM_BANK = CODE_BANK;
            if (!shopQuestion(KEY_PRICE, STRING_GAME_MSG_KEY)) {
                RAM_BANK = MAP_BANK;
                return 1;
            }
            RAM_BANK = MAP_BANK;

            players[playerId].gold -= KEY_PRICE;
        } else {
            players[playerId].score += KEY_SCORE;
        }

        soundPlaySFX(SOUND_SFX_KEY_GET, playerId);
        players[playerId].keys += 1;
        clearTile = 1;
    } else if (tile == TILE_TREASURE_CHEST || tile == TILE_TREASURE_GOLD || tile == TILE_TREASURE_SILVER) {
        if (!hints.treasure) {
            hints.treasure = 1;
            RAM_BANK = CODE_BANK;
            gameMessage(STRING_GAME_MSG_COLLECT_TREASURE, SOUND_MUSIC_TREASURE);
            RAM_BANK = MAP_BANK;
        }

        soundPlaySFX(SOUND_SFX_KEY_GET, playerId);
        players[playerId].gold += tile == TILE_TREASURE_CHEST ? TREASURE_CHEST_GOLD : tile == TILE_TREASURE_GOLD ? GOLD_PILE_GOLD : SILVER_PILE_GOLD;
        players[playerId].score += tile == TILE_TREASURE_CHEST ? TREASURE_CHEST_SCORE : tile == TILE_TREASURE_GOLD ? GOLD_PILE_SCORE : SILVER_PILE_SCORE;
        clearTile = 1;
    } else if (tile == TILE_SCROLL && (players[playerId].keys + players[playerId].scrolls < INVENTORY_LIMIT)) {
        if (!hints.scrolls) {
            hints.scrolls = 1;
            RAM_BANK = CODE_BANK;
            gameMessage(STRING_GAME_MSG_USE_SCROLLS, SOUND_MUSIC_SCROLLS);
            RAM_BANK = MAP_BANK;
        }

        if (isShopLevel) {
            if (players[playerId].gold < SCROLL_PRICE) {
                cannotAfford(playerId);
                return 1;
            }

            RAM_BANK = CODE_BANK;
            if (!shopQuestion(SCROLL_PRICE, STRING_GAME_MSG_SCROLL)) {
                RAM_BANK = MAP_BANK;
                return 1;
            }
            RAM_BANK = MAP_BANK;

            players[playerId].gold -= SCROLL_PRICE;
        } else {
            players[playerId].score += SCROLL_SCORE;
        }

        soundPlaySFX(SOUND_SFX_WEAPON_SWOOSH, playerId);
        players[playerId].scrolls += 1;
        clearTile = 1;
    } else if (tile >= TILE_BOOST_START && tile <= TILE_BOOST_END) {
        id = tile-TILE_BOOST_START;

        if (!hints.boosts[id]) {
            hints.boosts[id] = 1;
            RAM_BANK = CODE_BANK;
            gameMessage(STRING_BOOSTS_START+(id*3)+1, 0);
            RAM_BANK = MAP_BANK;
        }

        // If a player's character type already has this power, then don't let them pick it up.
        // If they alreay have this boost, then they can get it again but it has no effect (other than scoring points)
        // Might do this in a 2 player game to grief the other player!
        if (!players[playerId].canBoost[id]) {
            RAM_BANK = CODE_BANK;
            gameMessage(STRING_GAME_MSG_ALREADY_GIFTED, 0);
            RAM_BANK = MAP_BANK;
            stopMove(playerId);
            return 1;
        }
        
        if (isShopLevel) {
            if (players[playerId].gold < BOOST_PRICE) {
                cannotAfford(playerId);
                return 1;
            }

            RAM_BANK = CODE_BANK;
            if (!shopQuestion(BOOST_PRICE, STRING_BOOSTS_START+(id*3))) {
                RAM_BANK = MAP_BANK;
                return 1;
            }
            RAM_BANK = MAP_BANK;

            players[playerId].gold -= BOOST_PRICE;
        } else {
            players[playerId].score += BOOST_SCORE;
        }

        soundPlaySFX(SOUND_SFX_KEY_GET, playerId);
        players[playerId].hasBoosts[id] = 1;
        clearTile = 1;
    } else if (tile == TILE_FOOD_BIG || tile == TILE_FOOD_SMALL) {
        if (!hints.food) {
            hints.food = 1;
            RAM_BANK = CODE_BANK;
            gameMessage(STRING_GAME_MSG_EAT_FOOD, SOUND_MUSIC_FOOD);
            RAM_BANK = MAP_BANK;
        }

        if (isShopLevel) {
            if (tile == TILE_FOOD_BIG) {
                if (players[playerId].gold < BIG_FOOD_PRICE) {
                    cannotAfford(playerId);
                    return 1;
                } else {
                    RAM_BANK = CODE_BANK;
                    if (!shopQuestion(BIG_FOOD_PRICE, STRING_GAME_MSG_LARGE_MEAL)) {
                        RAM_BANK = MAP_BANK;
                        return 1;
                    }
                    RAM_BANK = MAP_BANK;

                    players[playerId].gold -= BIG_FOOD_PRICE;
                }
            }

            if (tile == TILE_FOOD_SMALL) {
                if (players[playerId].gold < SMALL_FOOD_PRICE) {
                    cannotAfford(playerId);
                    return 1;
                } else {
                    RAM_BANK = CODE_BANK;
                    if (!shopQuestion(SMALL_FOOD_PRICE, STRING_GAME_MSG_SMALL_MEAL)) {
                        RAM_BANK = MAP_BANK;
                        return 1;
                    }
                    RAM_BANK = MAP_BANK;

                    players[playerId].gold -= SMALL_FOOD_PRICE;
                }
            }
        } else {
            players[playerId].score += tile == TILE_FOOD_BIG ? 250 : 100;
        }

        soundPlaySFX(SOUND_SFX_EATING, playerId);
        players[playerId].health += tile == TILE_FOOD_BIG ? players[playerId].stats->foodHealthBig : players[playerId].stats->foodHealthSmall;
        clearTile = 1;
    } else if (tile == TILE_DOOR && players[playerId].keys > 0) {
        players[playerId].keys -= 1;
        clearTile = 1;

        i=1;
        while(mapStatus[toTileY-i][toTileX] == TILE_DOOR) {
            mapStatus[toTileY-i][toTileX] = TILE_FLOOR;
            copyTile(fromX, fromY, toTileX, toTileY-i);
            i++;
        }

        i=1;
        while(mapStatus[toTileY+i][toTileX] == TILE_DOOR) {
            mapStatus[toTileY+i][toTileX] = TILE_FLOOR;
            copyTile(fromX, fromY, toTileX, toTileY+i);
            i++;
        }

        i=1;
        while(mapStatus[toTileY][toTileX-i] == TILE_DOOR) {
            mapStatus[toTileY][toTileX-i] = TILE_FLOOR;
            copyTile(fromX, fromY, toTileX-i, toTileY);
            i++;
        }

        i=1;
        while(mapStatus[toTileY][toTileX+i] == TILE_DOOR) {
            mapStatus[toTileY][toTileX+i] = TILE_FLOOR;
            copyTile(fromX, fromY, toTileX+i, toTileY);
            i++;
        }
    } else if (tile == TILE_EXIT_1 || tile == TILE_EXIT_5 || tile == TILE_EXIT_10) {
        players[playerId].exit = tile;
        mapStatus[toTileY][toTileX+i] = TILE_FLOOR;
        toggleSprite(players[playerId].spriteAddrLo, players[playerId].spriteAddrHi, 0);
    } else if (tile == TILE_TELEPORTER) { 
        players[playerId].teleportTileX = toTileX;
        players[playerId].teleportTileY = toTileY;
        return 1;
    } else if (tile > TILE_FLOOR && tile < ENTITY_TILE_START) {
        return 1;
    } else if (tile >= GUY_CLAIM && tile != GUY_CLAIM+playerId) {
        // Don't collide with other player
        return 1;
    }

    if (clearTile) {
        overlayChanged = 1;
        mapStatus[toTileY][toTileX] = TILE_FLOOR;
        copyTile(fromX, fromY, toTileX, toTileY);
    }

    return 0;
}

#pragma code-name (push, "BANKRAM01")

void teleportPlayer(unsigned char playerId) {
    unsigned char t=0, toTileX, toTileY, foundSpot, entityTileX=0, entityTileY=0;
    signed char ty, tx;
    Entity *entity;

    while(t < 16) {
        t+= 1;
        for (ty=players[playerId].teleportTileY-t; ty<=players[playerId].teleportTileY+t && t<16; ty++) {
            for (tx=players[playerId].teleportTileX-t; tx<=players[playerId].teleportTileX+t && t<16; tx++) {
                // Only look at the "oustides" of this loop
                if (tx > 0 && tx < 31 && ty > 0 && ty < 31 &&
                    (ty==players[playerId].teleportTileY-t || ty==players[playerId].teleportTileY+t ||
                    tx==players[playerId].teleportTileX-t || tx==players[playerId].teleportTileX+t)) {
                    
                    // See if this tile is a teleporter
                    // Take the player there if so
                    if (mapStatus[ty][tx] == TILE_TELEPORTER) {
                        t = 16; // exit condition

                        toTileX = tx;
                        toTileY = ty;
                    }
                }
            }    
        }
    }

    // First try to land on a tile near the destination in the direction the player was moving
    t = mapStatus[toTileY+players[playerId].aimY][toTileX+players[playerId].aimX];

    if (t == TILE_FLOOR || (t >= ENTITY_TILE_START && t <= ENTITY_TILE_END)) {
        if (t >= ENTITY_TILE_START && t <= ENTITY_TILE_END) {
            // If there is an entity there...stomp on it!
            entity = getEntityById(t-ENTITY_TILE_START, entityActiveList);
            if (entity) {
                attackEntity(playerId, entity, 255);
            }
        }
        // Move the player
        players[playerId].x = (toTileX+players[playerId].aimX)*16;
        players[playerId].y = (toTileY+players[playerId].aimY)*16;
    } else {
        foundSpot = 0;
        // Need to find an empty tile around the destination
        // First look for empty tiles
        for (ty=toTileY-1; ty<=toTileY+1 && !foundSpot; ty++) {
            for (tx=toTileX-1; tx<=toTileX+1 && !foundSpot; tx++) {
                t = mapStatus[ty][tx];
                if (t == TILE_FLOOR) {
                    // Move the player
                    players[playerId].x = tx*16;
                    players[playerId].y = ty*16;
                    foundSpot = 1;
                } else if (!entityTileX && t >= ENTITY_TILE_START && t <= ENTITY_TILE_END) {
                    // If this spot is an entity, hold onto it as a backup if we can't find any empty tiles
                    entityTileX = tx;
                    entityTileY = ty;
                }
            }
        }

        // No empty tiles, stomp on an entity!
        if (!foundSpot && entityTileX) {
            t = mapStatus[entityTileY][entityTileX];
            entity = getEntityById(t-ENTITY_TILE_START, entityActiveList);
            if (entity) {
                attackEntity(playerId, entity, 255);
            }

            // Move the player
            players[playerId].x = entityTileX*16;
            players[playerId].y = entityTileY*16;
        }
    }

    players[playerId].teleportTileX = 0;
    players[playerId].teleportTileY = 0;

    soundPlaySFX(SOUND_SFX_TELEPORT, playerId);
}

#pragma code-name (pop)

void moveGuy(unsigned char playerId, unsigned char speed) {
    unsigned short prevX, prevY, shot = 0;
    unsigned char tile, tempTileX, tempTileY;
    signed char dirX = 0, dirY = 0;
    Entity *entity;

    players[playerId].currentTileX = (players[playerId].x+8)>>4;
    players[playerId].currentTileY = (players[playerId].y+8)>>4;

    prevX = players[playerId].x;
    prevY = players[playerId].y;

    if (players[playerId].pressedX == -1) {
        if (!players[playerId].pressedShoot) {
            dirX = -1;
            players[playerId].x -= speed;
        }
        players[playerId].facingX = 1;
    } else if (players[playerId].pressedX == 1) {
        if (!players[playerId].pressedShoot) {
            dirX = 1;
            players[playerId].x += speed;
        }
        players[playerId].facingX = 0;
    }

    if (dirX == 1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x+15, players[playerId].y)) {
            dirX = 0;
            players[playerId].x = prevX;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x+15, players[playerId].y+15)) {
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
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x, players[playerId].y+15)) {
                dirX = 0;
                players[playerId].x = prevX;
            }
        }
    }

    if (!players[playerId].pressedShoot) {
        if (players[playerId].pressedY == -1) {
            dirY = -1;
            players[playerId].y -= speed;
        } else if (players[playerId].pressedY == 1) {
            dirY = 1;
            players[playerId].y += speed;
        }
    }

    if (dirY == 1) {
        // Check if new tile is open...move back if not
        if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x+15, players[playerId].y+15)) {
            dirY = 0;
            players[playerId].y = prevY;
        } else {
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x, players[playerId].y+15)) {
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
            if (tryTile(playerId, players[playerId].currentTileX, players[playerId].currentTileY, players[playerId].x+15, players[playerId].y)) {
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

    // Reset the pressed directions if blocked
    // Some monsters use these to aim
    players[playerId].pressedX = dirX;
    players[playerId].pressedY = dirY;

    // Check if melee attacking
    if (!players[playerId].pressedShoot) {
        tile = mapStatus[((players[playerId].y+8)+(dirY*8))>>4][((players[playerId].x+8)+(dirX*8))>>4];
        if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
            if (players[playerId].ticksUntilNextMelee == 0) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    shot = 1; // Just used to trigger attack animation
                    players[playerId].ticksUntilNextMelee = players[playerId].stats->ticksToMelee + 1;
                    attackEntity(playerId, entity, players[playerId].hasBoosts[BOOST_ID_MELEE]
                        ? players[playerId].boostedStats->meleeDamage
                        : players[playerId].stats->meleeDamage);
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

    // See if player is teleporting
    if (players[playerId].teleportTileX) {
        teleportPlayer(playerId);
    }

    tempTileX = (players[playerId].x+8)>>4;
    tempTileY = (players[playerId].y+8)>>4;

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
        soundPlaySFX(players[playerId].weaponSound, playerId);
        players[playerId].ticksUntilNextShot = players[playerId].stats->ticksToRanged;
        weapons[playerId].x = players[playerId].x;
        weapons[playerId].y = players[playerId].y;
        weapons[playerId].visible = 1;
        weapons[playerId].dirX = players[playerId].aimX;
        weapons[playerId].dirY = players[playerId].aimY;

        // Adjust some diagonal shots by 1 pixel on x to make sure they can go through corners
        if ((weapons[playerId].dirX == -1 && weapons[playerId].dirY == 1) ||
            (weapons[playerId].dirX == 1 && weapons[playerId].dirY == -1)) {
                weapons[playerId].x -= 1;
            }
        weapons[playerId].animationCount = WEAPON_ROTATION_SPEED;
        weapons[playerId].animationFrame = 0;

        toggleSprite(weapons[playerId].spriteAddrLo, weapons[playerId].spriteAddrHi, 1);

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
        tile = mapStatus[(weapons[playerId].y+8)>>4][(weapons[playerId].x+8)>>4];
        if (tile != TILE_FLOOR && tile != GUY_CLAIM+playerId && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleSprite(weapons[playerId].spriteAddrLo, weapons[playerId].spriteAddrHi, 0);

            if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    attackEntity(playerId, entity, players[playerId].hasBoosts[BOOST_ID_RANGED]
                        ? players[playerId].boostedStats->rangedDamage
                        : players[playerId].stats->rangedDamage);
                }
            }
            return;
        }

        weapons[playerId].x += weapons[playerId].dirX * WEAPON_SPEED;
        weapons[playerId].y += weapons[playerId].dirY * WEAPON_SPEED;

        // See if weapon is off screen
        if (weapons[playerId].x >= scrollX + SCROLL_PIXEL_SIZE || weapons[playerId].x+16 <= scrollX || weapons[playerId].y >= scrollY + SCROLL_PIXEL_SIZE || weapons[playerId].y+16 <= scrollY) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleSprite(weapons[playerId].spriteAddrLo, weapons[playerId].spriteAddrHi, 0);
            return;
        }

        moveAndSetAnimationFrame(weapons[playerId].spriteAddrLo, weapons[playerId].spriteAddrHi, weapons[playerId].spriteGraphicLo, weapons[playerId].spriteGraphicHi,
            weapons[playerId].x, weapons[playerId].y, 0, weaponRotation[weapons[playerId].animationFrame], 0);

        // Check if hit something after the move
        tile = mapStatus[(weapons[playerId].y+8)>>4][(weapons[playerId].x+8)>>4];
        if (tile != TILE_FLOOR && tile != GUY_CLAIM+playerId && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleSprite(weapons[playerId].spriteAddrLo, weapons[playerId].spriteAddrHi, 0);

            if (tile >= ENTITY_TILE_START && tile <= ENTITY_TILE_END) {
                entity = getEntityById(tile-ENTITY_TILE_START, entityActiveList);
                if (entity) {
                    attackEntity(playerId, entity, players[playerId].hasBoosts[BOOST_ID_RANGED]
                        ? players[playerId].boostedStats->rangedDamage
                        : players[playerId].stats->rangedDamage);
                }
            }
        }
    }
}

unsigned char destroyPlayer(unsigned char playerId) {
    unsigned char i, entityId;

    players[playerId].health = 0;
    players[playerId].active = 0;
    activePlayers -= 1;
    toggleSprite(players[playerId].spriteAddrLo, players[playerId].spriteAddrHi, 0);

    for (i=0; i < ENTITY_COUNT; i++) {
        if (entityList[i].health == 0) {
            createEntity(TILE_GENERATOR_START, i, players[playerId].currentTileX, players[playerId].currentTileY, 0);
            addNewEntityToList(&entityList[i], &entitySleepList);
            entityId = i;
            break;
        }
    }

    // Player died, retarget all entities
    for (i=0; i < ENTITY_COUNT; i++) {
        entityList[i].hasTarget = 0;        
    }

    soundPlayMusic(SOUND_MUSIC_LAUGH);

    return entityId;
}

#pragma code-name (push, "BANKRAM02")

void setupPlayer(unsigned char playerId, enum Character characterType) {
    unsigned char i;
    unsigned long spriteGraphicAddress;

    players[playerId].active = 1;
    players[playerId].characterType = characterType;
    players[playerId].stats =  playerStatsByType[characterType];
    players[playerId].boostedStats =  playerBoostedStatsByType[characterType];
    players[playerId].health = players[playerId].stats->startingHealth;
    players[playerId].animationCount = ANIMATION_FRAME_SPEED;
    players[playerId].animationFrame = 0;
    players[playerId].animationTile = GUY_TILE_START + (playerId*20) + (characterType*5);
    players[playerId].weaponTile = GUY_TILE_START + (playerId*20) + (characterType*5) + 4;
    players[playerId].ticksUntilNextMelee = 0;
    players[playerId].ticksUntilNextShot = 0;
    players[playerId].shooting = 0;
    players[playerId].aimX = 1;
    players[playerId].aimY = 0;
    players[playerId].facingX = 0;
    players[playerId].score = 0;
    players[playerId].gold = PLAYER_STARTING_GOLD;
    players[playerId].keys = 0;
    players[playerId].scrolls = 0;
    players[playerId].exit = 0;
    players[playerId].animationChange = 1; // Trigger immediate animation
    players[playerId].wasHit = 0;
    players[playerId].weaponSound = characterType == BARBARIAN || characterType == RANGER
        ? SOUND_SFX_WEAPON_SWOOSH : SOUND_SFX_WEAPON_BLAST;

    for (i=0; i<5; i++) {
        players[playerId].hasBoosts[i] = 0;
    }

    players[playerId].canBoost = playerCanBoostByType[characterType];

    spriteGraphicAddress = L0_TILEBASE_ADDR + (players[playerId].animationTile*L0_TILE_SIZE);

    players[playerId].spriteGraphicLo = spriteGraphicAddress>>5;
    players[playerId].spriteGraphicHi = spriteGraphicAddress>>13;

    // Weapon graphic
    spriteGraphicAddress = L0_TILEBASE_ADDR + (players[playerId].weaponTile*L0_TILE_SIZE);

    weapons[playerId].spriteGraphicLo = spriteGraphicAddress>>5;
    weapons[playerId].spriteGraphicHi = spriteGraphicAddress>>13;

    overlayChanged = 1;
}

#pragma code-name (pop)
