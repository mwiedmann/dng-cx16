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

void setScroll() {
    unsigned short x, y;

    // If only 1 player active, use it
    if (!players[1].active) {
        x = players[0].x;
        y = players[0].y;
    } else if (!players[0].active) {
        x = players[1].x;
        y = players[1].y;
    } else {
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
    
    VERA.layer0.vscroll = scrollY;
    VERA.layer0.hscroll = scrollX;
}

void main() {
    unsigned char count = 0, load, level, exitLevel, gameOver, i;
    unsigned char inputTicks = 0;
    Entity *entity;

    init();
    initTiles();

    while(1) {
        gameOver=0;
        level=0;

        scrollX=0;
        scrollY=0;
        VERA.layer0.vscroll = scrollY;
        VERA.layer0.hscroll = scrollX;
        
        entityActiveList = 0;
        entitySleepList = 0;

        spritesConfig();
        clearLayers();

        // Only player 0 for now
        setupPlayer(0, BARBARIAN);
        setupPlayer(1, DRUID);
        players[1].active = 0;

        drawOverlayBackground();
        updateOverlay();
        instructions();

        while(!gameOver) {
            createMapStatus(level);
            drawMap(level);
            exitLevel = 0;

            while(!exitLevel && !gameOver) {
                // Get joystick input only periodically
                if (inputTicks == 4) {
                    for (i=0; i<NUM_PLAYERS; i++) {
                        if (!players[i].active) {
                            continue;
                        }
                        setGuyDirection(i);
                    }
                    inputTicks = 0;
                } else {
                    inputTicks++;
                }

                for (i=0; i<NUM_PLAYERS; i++) {
                    if (!players[i].active) {
                        continue;
                    }
                    moveGuy(i, players[i].stats->speeds[count]);
                }

                setScroll();

                // Only set his animation frame if needed (this is more expensive)
                // Otherwise just move him
                for (i=0; i<NUM_PLAYERS; i++) {
                    if (!players[i].active) {
                        continue;
                    }

                    if (players[i].animationChange) {
                        players[i].animationChange = 0;
                        moveAndSetAnimationFrame(i, players[i].x, players[i].y, scrollX, scrollY, players[i].animationTile, players[i].animationFrame, players[i].facingX);
                    } else {
                        moveSpriteId(i, players[i].x, players[i].y, scrollX, scrollY);
                    }

                    moveWeapon(i);
                }

                if (count == 0 || count == 2) {
                    // activation/deactivation phase
                    activateEntities(scrollX, scrollY);
                    deactivateEntities(scrollX, scrollY);
                    tempActiveToActiveEntities();

                    // Move "some" active entities
                    // Try to split the load a bit
                    entity = entityActiveList;
                    load=0;
                    while(entity && load<15) {
                        moveEntity(entity, scrollX, scrollY);   
                        entity->movedPrevTick=1;
                        entity = entity->next;
                        load++;
                    };
                } else {
                    // Move active entities phase
                    entity = entityActiveList;

                    while(entity) {
                        if (!entity->movedPrevTick) {
                            moveEntity(entity, scrollX, scrollY);   
                        } else {
                            entity->movedPrevTick = 0;
                        }
                        entity = entity->next;
                    };

                    if (overlayChanged) {
                        updateOverlay();
                        overlayChanged = 0;
                    }
                }

                count++;
                if (count == 4) {
                    count = 0;
                }

                exitLevel = 1;
                gameOver = 1;
                for (i=0; i<NUM_PLAYERS; i++) {
                    if (players[i].active) {
                        gameOver = 0;
                        if (!players[i].exit) {
                            exitLevel = 0;
                        }
                    }
                }
                wait();
            }

            level++;

            for (i=0; i<NUM_PLAYERS; i++) {
                players[i].exit = 0;
            }

            // Cleanup active entities
            entity = entityActiveList;
            while(entity) {
                toggleEntity(entity->spriteId, 0);
                entity = entity->next;
            };

            entityActiveList = 0;
            entitySleepList = 0;
        }
    }
}