#include <cx16.h>
#include <joystick.h>
#include <stdio.h>
#include <6502.h>

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
#include "strtbl.h"

#define IRQ_HANDLER_STACK_SIZE 8
unsigned char irqHandlerStack[IRQ_HANDLER_STACK_SIZE];

unsigned char scrollMode = 0;

unsigned char irqHandler() {
    if (!scrollMode) {
        waitStatus = 1;
        return IRQ_HANDLED;
    }
    
    VERA.layer0.vscroll = scrollY;
    VERA.layer0.hscroll = scrollX;

    waitStatus = 1;
    return IRQ_HANDLED;
}

#pragma code-name (push, "BANKRAM02")

void prepPlayersForLevel() {
    unsigned char i,j;

    // Show the active players
    for (i=0; i<NUM_PLAYERS; i++) {
        // All boosted stats are reset when reaching a shop level
        if (isShopLevel) {
            for (j=0; j<5; j++) {
                players[i].hasBoosts[j] = 0;
            }
            overlayChanged = 1;
        }

        if (players[i].active) {
            toggleSprite(players[i].spriteAddrLo, players[i].spriteAddrHi, 1);
        }
    }
}

#pragma code-name (pop)

void setScroll() {
    unsigned short x, y;

    if (activePlayers == 1) {
        // If only 1 player active, use it
        if (players[0].active && !players[0].exit) {
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
}

void main() {
    unsigned char count = 0, load, exitLevel, gameOver, i, healthTicks, deadCount;
    unsigned short doorTicks;
    unsigned char inputTicks = 0;
    Entity *entity;

    // Setup the IRQ handler vsync
    set_irq(&irqHandler, irqHandlerStack, IRQ_HANDLER_STACK_SIZE);
    VERA.irq_enable = 1;

    toggleLayers(0);
    loadStrings();
    loadBankedCode();

    RAM_BANK = CODE_BANK;
    showTitle();
    soundInit();

#ifndef TEST_MODE    
    waitCount(60);
    soundPlayMusic(SOUND_MUSIC_TITLE);

    // Sound switches the bank...switch back
    RAM_BANK = CODE_BANK;
    waitForButtonPress();
#else
    RAM_BANK = CODE_BANK;
#endif

    toggleLayers(0);
    init();
    initTiles();

    while(1) {
#ifndef TEST_MODE        
        soundPlayMusic(SOUND_MUSIC_TITLE);
#endif
        RAM_BANK = CODE_BANK;

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

        soundStopChannel(SOUND_PRIORITY_MUSIC);
        
        while(!gameOver) {
            activePlayers=0;
            for (i=0; i<NUM_PLAYERS; i++) {
                if (players[i].active) {
                    activePlayers+= 1;
                }
            }

            // Shop levels are every 5 levels
            // Items must be purchased on these levels
            isShopLevel = level % 5 == 0;

            RAM_BANK = CODE_BANK;
            loadDungeonTiles();

#ifndef TEST_MODE
            showLevelIntro();
#endif
            clearBank(MAP_BANK, MAP_BANK_CODE_SIZE);
            createMapStatus(level);

            RAM_BANK = CODE_BANK;
            drawMap(level);
            RAM_BANK = MAP_BANK;

#ifndef TEST_MODE
            if (level == STARTING_LEVEL) {
                soundPlayMusic(SOUND_MUSIC_WELCOME);
            }
#endif
            
            exitLevel = 0;
            healthTicks = 0;
            doorTicks = 0;

            RAM_BANK = CODE_BANK;
            prepPlayersForLevel();
            RAM_BANK = MAP_BANK;

            scrollMode = 1;

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

                // This will set the scrolling based on the player positions
                // We want to set this before positioning the sprites because
                // they are positioned relative to the final scroll position 
                setScroll();

                // Only set his animation frame if needed (this is more expensive)
                // Otherwise just move him
                for (i=0; i<NUM_PLAYERS; i++) {
                    if (!players[i].active || players[i].exit) {
                        continue;
                    }

                    if (players[i].wasHit || players[i].animationChange) {
                        players[i].animationChange = 0;
                        moveAndSetAnimationFrame(players[i].spriteAddrLo, players[i].spriteAddrHi, players[i].spriteGraphicLo, players[i].spriteGraphicHi,
                            players[i].x, players[i].y, players[i].animationFrame, players[i].facingX, players[i].wasHit > PLAYER_HIT_ANIM_FRAMES_STOP);
                        
                        if (players[i].wasHit) {
                            if (!demonSoundOn && healthTicks % 30 == 0) {
                                soundPlaySFX(SOUND_SFX_DAMAGE, SOUND_PRIORITY_COMMON);
                            }
                            players[i].wasHit -= 1;
                        }
                    } else {
                        moveSprite(players[i].spriteAddrLo, players[i].spriteAddrHi, players[i].x, players[i].y);
                    }

                    moveWeapon(i);
                }

                if (demonHitting) {
                    demonHitting -= 1;
                    if (!demonSoundOn) {
                        demonSoundOn = 1;
                        soundPlaySFX(SOUND_SFX_DEMON, SOUND_PRIORITY_COMMON);
                    }
                } else if (!demonHitting && demonSoundOn) {
                    demonSoundOn = 0;
                    soundStopChannel(SOUND_PRIORITY_COMMON);
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
                    while(entity && (load < (activePlayers == 1 ? ENTITY_SPLIT_AMOUNT_1P : ENTITY_SPLIT_AMOUNT_2P))) {
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
                        RAM_BANK = CODE_BANK;
                        updateOverlay();
                        RAM_BANK = MAP_BANK;
                        overlayChanged = 0;
                    }
                }

                // Player slowly loses health unless in a shop
                if (!isShopLevel) {
                    healthTicks++;
                    if (healthTicks == 60) {
                        healthTicks = 0;
                        for (i=0; i<NUM_PLAYERS; i++) {
                            if (players[i].active && !players[i].exit) {
                                meleeAttackGuy(i, 0, 1, 0);
                            }
                        }
                    }

                    doorTicks++;
                    if (doorTicks == OPEN_ALL_DOORS_TICKS) {
                        for (i=0; i<NUM_PLAYERS; i++) {
                            if (players[i].active && !players[i].exit) {
                                openAllDoors(players[i].currentTileX, players[i].currentTileY);
                                break;
                            }
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
                    if (deadCount < 250) {
                        gameOver = 0;
                        exitLevel = 0;
                    }
                }
                
                wait();
            }

            // Players can go to next level or jump 5 or 10 levels
            level += exitLevel == TILE_EXIT_1 ? 1 : exitLevel == TILE_EXIT_5 ? 5 : 10;
            if (level > LEVEL_COUNT) {
                level = 1; // warp back to level 1 after last level (for now)
            }
            
            for (i=0; i<NUM_PLAYERS; i++) {
                players[i].exit = 0;
            }

            // Cleanup all entities
            for (i=0; i < ENTITY_COUNT; i++) {
                if (entityList[i].spriteId) {
                    toggleSprite(entityList[i].spriteAddrLo, entityList[i].spriteAddrHi, 0);
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
            scrollMode = 0;
            VERA.layer0.vscroll = scrollY;
            VERA.layer0.hscroll = scrollX;
        }
    }
}