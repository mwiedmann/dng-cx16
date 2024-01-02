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

void main() {
    unsigned char count = 0, load, level=0, exitLevel=0, gameOver=0, i;
    unsigned char inputTicks = 0;
    Entity *entity;

    init();
    initTiles();
    spritesConfig();
    clearLayers();

    // Only player 0 for now
    players[1].active = 0;
    players[1].characterType = BARBARIAN;

    setupPlayer(0, BARBARIAN);

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

            // TODO: Scroll between 2 players if both active
            // Just follow P0 for now
            scrollX = players[0].x-112;
            if (scrollX < 0) {
                scrollX = 0;
            } else if (scrollX > maxMapX) {
                scrollX = maxMapX;
            }

            scrollY = players[0].y-112;
            if (scrollY < 0) {
                scrollY = 0;
            } else if (scrollY > maxMapY) {
                scrollY = maxMapY;
            }
            
            VERA.layer0.vscroll = scrollY;
            VERA.layer0.hscroll = scrollX;

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
                while(entity && load<10) {
                    moveEntity(entity, players[0].currentTileX, players[0].currentTileY, scrollX, scrollY);   
                    entity->movedPrevTick=1;
                    entity = entity->next;
                    load++;
                };
            } else {
                // Move active entities phase
                entity = entityActiveList;

                while(entity) {
                    if (!entity->movedPrevTick) {
                        moveEntity(entity, players[0].currentTileX, players[0].currentTileY, scrollX, scrollY);   
                    } else {
                        entity->movedPrevTick = 0;
                    }
                    entity = entity->next;
                };

                // TODO: Only need to update this periodically
                updateOverlay();
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
    }
}