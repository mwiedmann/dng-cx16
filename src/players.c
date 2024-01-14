#include <cx16.h>
#include <joystick.h>
#include <stdio.h>

#include "globals.h"
#include "tiles.h"
#include "sprites.h"
#include "list.h"
#include "ai.h"

char *boostHints[5][2] = {
    {"MUSHROOM INCREASES", "PLAYER SPEED"},
    {"WEAPON IMPROVES", "PLAYER MELEE ATTACKS"},
    {"WEAPON IMPROVES", "PLAYER RANGED ATTACKS"},
    {"RING IMPROVES", "PLAYER SCROLL POWER"},
    {"MITHRIL IMPROVES", "PLAYER ARMOR"}
};

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

#define KEY_PRICE 250
#define SCROLL_PRICE 500
#define BIG_FOOD_PRICE 500
#define SMALL_FOOD_PRICE 250

#define BOOST_PRICE 1000

unsigned char tryTile(unsigned char playerId, unsigned char fromX, unsigned char fromY, unsigned short toX, unsigned short toY) {
    unsigned char toTileX = toX>>4;
    unsigned char toTileY = toY>>4;
    unsigned char tile = mapStatus[toTileY][toTileX];
    signed char i;
    unsigned char clearTile=0, id;
    char buf[30];

    // Scrolling is split between both players in 2 player game
    // Make sure players aren't leaving the scroll field
    if (activePlayers == 2) {
        if (toX >= scrollX + TWO_PLAYER_SCROLL_LIMIT || toX <= scrollX+16 || toY >= scrollY + TWO_PLAYER_SCROLL_LIMIT || toY <= scrollY+16) {
            return 1;
        }
    }

    // There is an inventory limit on keys+scrolls
    if (tile == TILE_KEY && (players[playerId].keys + players[playerId].scrolls < INVENTORY_LIMIT)) {
        if (isShopLevel) {
            if (players[playerId].gold < KEY_PRICE) {
                return 1;
            }

            sprintf(buf, "%u GOLD", KEY_PRICE);
            if (gameQuestion("PURCHASE KEY FOR", buf)) {
                players[playerId].gold -= KEY_PRICE;
            } else {
                return 1;
            }
        } else {
            if (!hints.keys) {
                hints.keys = 1;
                gameMessage("COLLECT KEYS", "TO OPEN DOORS");
            }

            players[playerId].score += 100;
        }

        players[playerId].keys += 1;
        clearTile = 1;
    } else if (tile == TILE_TREASURE_CHEST || tile == TILE_TREASURE_GOLD || tile == TILE_TREASURE_SILVER) {
        if (!hints.treasure) {
            hints.treasure = 1;
            gameMessage("COLLECT GOLD AND", "SPEND ON UPGRADES LATER");
        }
        players[playerId].gold += tile == TILE_TREASURE_CHEST ? 500 : tile == TILE_TREASURE_GOLD ? 250 : 100;
        players[playerId].score += tile == TILE_TREASURE_CHEST ? 500 : tile == TILE_TREASURE_GOLD ? 250 : 100;
        clearTile = 1;
    } else if (tile == TILE_SCROLL && (players[playerId].keys + players[playerId].scrolls < INVENTORY_LIMIT)) {
        if (isShopLevel) {
            if (players[playerId].gold < SCROLL_PRICE) {
                return 1;
            }

            players[playerId].gold -= SCROLL_PRICE;
        } else {
            if (!hints.scrolls) {
                hints.scrolls = 1;
                gameMessage("USE SCROLLS TO", "DAMAGE ALL ENEMIES");
            }
            players[playerId].score += 250;
        }

        players[playerId].scrolls += 1;
        clearTile = 1;
    } else if (tile >= TILE_BOOST_START && tile <= TILE_BOOST_END) {
        id = tile-TILE_BOOST_START;

        if (!players[playerId].canBoost[id]) {
            return 1;
        }

        if (isShopLevel) {
            if (players[playerId].gold < BOOST_PRICE) {
                return 1;
            }

            players[playerId].gold -= BOOST_PRICE;
        } else {
            players[playerId].score += 250;
        }

        if (!hints.boosts[id]) {
            hints.boosts[id] = 1;
            gameMessage(boostHints[id][0], boostHints[id][1]);
        }

        players[playerId].hasBoosts[id] = 1;
        clearTile = 1;
    } else if (tile == TILE_FOOD_BIG || tile == TILE_FOOD_SMALL) {
        if (isShopLevel) {
            if (tile == TILE_FOOD_BIG) {
                if (players[playerId].gold < BIG_FOOD_PRICE) {
                    return 1;
                } else {
                    players[playerId].gold -= BIG_FOOD_PRICE;
                }
            }

            if (tile == TILE_FOOD_SMALL) {
                if (players[playerId].gold < SMALL_FOOD_PRICE) {
                    return 1;
                } else {
                    players[playerId].gold -= SMALL_FOOD_PRICE;
                }
            }
        } else {
            if (!hints.food) {
                hints.food = 1;
                gameMessage("EAT FOOD TO", "GAIN HEALTH");
            }
            players[playerId].score += tile == TILE_FOOD_BIG ? 250 : 100;
        }

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
        toggleEntity(playerId, 0);
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
            toggleWeapon(playerId, 0);

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
            toggleWeapon(playerId, 0);
            return;
        }

        moveAndSetAnimationFrame(WEAPON_SPRITE_ID_START+playerId, weapons[playerId].x, weapons[playerId].y, players[playerId].weaponTile, 0, weaponRotation[weapons[playerId].animationFrame]);

        // Check if hit something after the move
        tile = mapStatus[(weapons[playerId].y+8)>>4][(weapons[playerId].x+8)>>4];
        if (tile != TILE_FLOOR && tile != GUY_CLAIM+playerId && tile != ENTITY_CLAIM) {
            // Hide it for now
            weapons[playerId].visible = 0;
            toggleWeapon(playerId, 0);

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

#pragma code-name (push, "BANKRAM02")

unsigned char destroyPlayer(unsigned char playerId) {
    unsigned char i, entityId;

    players[playerId].health = 0;
    players[playerId].active = 0;
    activePlayers -= 1;
    toggleEntity(playerId, 0);

    for (i=0; i < ENTITY_COUNT; i++) {
        if (entityList[i].health == 0) {
            createEntity(TILE_GENERATOR_START, i, players[playerId].currentTileX, players[playerId].currentTileY);
            addNewEntityToList(&entityList[i], &entitySleepList);
            entityId = i;
            break;
        }
    }

    // Player died, retarget all entities
    for (i=0; i < ENTITY_COUNT; i++) {
        entityList[i].hasTarget = 0;        
    }

    return entityId;
}

void setupPlayer(unsigned char playerId, enum Character characterType) {
    unsigned char i;

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
    players[playerId].gold = 0;
    players[playerId].keys = 0;
    players[playerId].scrolls = 0;
    players[playerId].exit = 0;
    players[playerId].animationChange = 1; // Trigger immediate animation

    for (i=0; i<5; i++) {
        players[playerId].hasBoosts[i] = 0;
    }

    players[playerId].canBoost = playerCanBoostByType[characterType];

    overlayChanged = 1;
}

#pragma code-name (pop)
