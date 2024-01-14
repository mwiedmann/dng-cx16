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
#include "players.h"
#include "intro.h"
#include "sound.h"

void setScroll() {
    unsigned short x, y;

    if (activePlayers == 1) {
        // If only 1 player active, use it
        if (!players[1].active) {
            x = players[0].x;
            y = players[0].y;
        } else {
            x = players[1].x;
            y = players[1].y;
        }
    } else if (activePlayers == 2) {
        // Both players active, scroll around the midpoint
        x = (players[0].x + players[1].x) >>1;
        y = (players[0].y + players[1].y) >>1;
    } else {
        // Nobody active...just keep the scroll where it is
        // Game is probably over
        return;
    }

    // Just follow P0 for now
    scrollX = x-112;
    if (scrollX < 0) {
        scrollX = 0;
    } else if (scrollX > maxMapX) {
        scrollX = maxMapX;
    }

    scrollY = y-112;
    if (scrollY < 0) {
        scrollY = 0;
    } else if (scrollY > maxMapY) {
        scrollY = maxMapY;
    }
    
    VERA.layer0.vscroll = scrollY;
    VERA.layer0.hscroll = scrollX;
}

void main() {
    unsigned char count = 0, load, exitLevel, gameOver, i, j, healthTicks, deadCount;
    unsigned char inputTicks = 0;
    Entity *entity;

    loadBankedCode();

    BANK_NUM = CODE_BANK;
    init();
    initTiles();

    // soundInit();
    // soundPlayMusic(SOUND_MUSIC_TITLE);

    // BANK_NUM = CODE_BANK;

    while(1) {
        BANK_NUM = CODE_BANK;
        
        gameOver=0;
        deadCount=0;
        level=STARTING_LEVEL;

        scrollX=0;
        scrollY=0;
        VERA.layer0.vscroll = scrollY;
        VERA.layer0.hscroll = scrollX;
        
        entityActiveList = 0;
        entitySleepList = 0;

        loadDungeonTiles();
        spritesConfig();

        players[0].active = 0;
        players[1].active = 0;

        instructions();

        activePlayers=0;
        for (i=0; i<NUM_PLAYERS; i++) {
            if (players[i].active) {
                activePlayers+= 1;
            }
        }

        while(!gameOver) {
            loadDungeonTiles();
            showLevelIntro();
            
            clearBank(MAP_BANK);
            createMapStatus(level);

            BANK_NUM = CODE_BANK;
            drawMap(level);
            BANK_NUM = MAP_BANK;

            exitLevel = 0;
            healthTicks = 0;

            // Shop levels are every 5 levels
            // Items need to be purchased on these levels
            isShopLevel = level != 0 && level % 5 == 0;

            // Show the active players
            for (i=0; i<NUM_PLAYERS; i++) {
                // All boosted stats are reset when reaching a shop level
                if (isShopLevel) {
                    for (j=0; j<5; j++) {
                        players[i].hasBoosts[j] = 0;
                    }
                }

                if (players[i].active) {
                    toggleEntity(i, 1);
                }
            }

            while(!exitLevel && !gameOver) {
                // Get joystick input only periodically
                if (inputTicks == 4) {
                    for (i=0; i<NUM_PLAYERS; i++) {
                        if (!players[i].active || players[i].exit) {
                            continue;
                        }
                        setGuyDirection(i);
                    }
                    inputTicks = 0;
                } else {
                    inputTicks++;
                }

                for (i=0; i<NUM_PLAYERS; i++) {
                    if (!players[i].active || players[i].exit) {
                        continue;
                    }
                    moveGuy(i, players[i].stats->speeds[count]);
                }

                setScroll();

                // Only set his animation frame if needed (this is more expensive)
                // Otherwise just move him
                for (i=0; i<NUM_PLAYERS; i++) {
                    if (!players[i].active || players[i].exit) {
                        continue;
                    }

                    if (players[i].animationChange) {
                        players[i].animationChange = 0;
                        moveAndSetAnimationFrame(i, players[i].x, players[i].y, players[i].animationTile, players[i].animationFrame, players[i].facingX);
                    } else {
                        moveSpriteId(i, players[i].x, players[i].y);
                    }

                    moveWeapon(i);
                }

                if (count == 0 || count == 2 || count == 4) {
                    // activation/deactivation phase
                    activateEntities();
                    deactivateEntities();
                    tempActiveToActiveEntities();

                    // Move "some" active entities
                    // Try to split the load a bit
                    entity = entityActiveList;
                    load=0;
                    while(entity && load<15) {
                        moveEntity(entity);   
                        entity->movedPrevTick=1;
                        entity = entity->next;
                        load++;
                    };
                } else {
                    // Move active entities phase
                    entity = entityActiveList;

                    while(entity) {
                        if (!entity->movedPrevTick) {
                            moveEntity(entity);   
                        } else {
                            entity->movedPrevTick = 0;
                        }
                        entity = entity->next;
                    };

                    if (overlayChanged) {
                        BANK_NUM = CODE_BANK;
                        updateOverlay();
                        BANK_NUM = MAP_BANK;
                        overlayChanged = 0;
                    }
                }

                // Player slowly loses health
                healthTicks++;
                if (healthTicks == 60) {
                    healthTicks = 0;
                    for (i=0; i<NUM_PLAYERS; i++) {
                        if (players[i].active && !players[i].exit) {
                            meleeAttackGuy(i, 0, 1);
                        }
                    }
                }
                
                count++;
                if (count == 6) {
                    count = 0;
                }

                exitLevel = 1;
                gameOver = 1;
                for (i=0; i<NUM_PLAYERS; i++) {
                    if (players[i].active) {
                        gameOver = 0;
                        if (players[i].exit) {
                            exitLevel = players[i].exit;
                        } else {
                            // 1 player has not exited...continue level
                            exitLevel = 0;
                            break;
                        }
                    }
                }

                // Let the game continue for a few seconds after the players are dead
                if (gameOver) {
                    deadCount+= 1;
                    if (deadCount < 180) {
                        gameOver = 0;
                        exitLevel = 0;
                    }
                }
                wait();
            }

            // Players can go to next level or jump 5 or 10 levels
            level += exitLevel == TILE_EXIT_1 ? 1 : exitLevel == TILE_EXIT_5 ? 5 : 10;

            
            for (i=0; i<NUM_PLAYERS; i++) {
                players[i].exit = 0;
            }

            // Cleanup all entities
            for (i=0; i < ENTITY_COUNT; i++) {
                if (entityList[i].spriteId) {
                    toggleEntity(entityList[i].spriteId, 0);
                }
                entityList[i].health = 0;
                entityList[i].prev = 0;
                entityList[i].next = 0;
            }

            entityActiveList = 0;
            entitySleepList = 0;
            entityTempActiveList = 0;

            // Reset scrolling
            scrollX=0;
            scrollY=0;
            VERA.layer0.vscroll = scrollY;
            VERA.layer0.hscroll = scrollX;
        }
    }
}