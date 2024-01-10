#include <cx16.h>
#include <joystick.h>

#include "tiles.h"
#include "globals.h"
#include "joy.h"
#include "players.h"
#include "wait.h"

/*
        BARBARIAN    MAGE    DRUID    RANGER
Speed      **        **      **       ****
Melee      ****      *       **       ***
Ranged
 Damage    ****      ***     **       **
 Rate      *         ****    **       ****
Magic      *         ****    ****     **
Health     ****      *       ***      **
Armor      **        *       ****     **
*/

#pragma code-name (push, "BANKRAM02")

void instructions1() {
    message(9, 0, "YOUR HEROES");

    message(2, 2, "BARBARIAN");
    l0TileShow(6, 1, GUY_TILE_START);

    message(19, 2, "MAGE");
    l0TileShow(8, 1, GUY_TILE_START+8);

    message(6, 5, "DRUID");
    l0TileShow(6, 2, GUY_TILE_START+16);

    message(19, 5, "RANGER");
    l0TileShow(8, 2, GUY_TILE_START+24);

    message(1, 8, "SHOOT OR MELEE ATTACK ENEMIES");
    l0TileShow(3, 5, 96);
    l0TileShow(5, 5, 100);
    l0TileShow(7, 5, 104);
    l0TileShow(9, 5, 108);
    l0TileShow(11, 5, 112);

    l0TileShow(3, 6, 116);
    l0TileShow(5, 6, 120);
    l0TileShow(7, 6, 124);
    l0TileShow(9, 6, 128);
    l0TileShow(11, 6, 132);

    l0TileShow(0, 8, 35);
    l0TileShow(1, 8, 36);
    l0TileShow(2, 8, 37);
    message(7, 16, "GET GOLD TO SPEND");
    message(7, 17, "ON UPGRADES LATER");

    l0TileShow(1, 10, 39);
    message(5, 20, "SCROLLS HIT ALL ENEMIES");

    l0TileShow(1, 11, 38);
    message(5, 22, "KEYS OPEN DOORS:");
    l0TileShow(11, 11, 30);
    l0TileShow(12, 11, 31);

    l0TileShow(1, 12, 40);
    l0TileShow(2, 12, 41);
    message(7, 25, "FOOD RESTORES HEALTH");

    message(5, 28, "PRESS A BUTTON TO START");
}

void instructions2() {
    unsigned char startY = 15;

    message (7, 2, "PRESS A DIRECTION");
    message (7, 3, "TO SELECT A HERO");

    message(11, 5, "BARBARIAN");
    l0TileShow(7, 3, GUY_TILE_START);

    message(5, 8, "DRUID");
    l0TileShow(5, 4, GUY_TILE_START+16);

    message(20, 8, "RANGER");
    l0TileShow(9, 4, GUY_TILE_START+24);

    message(13, 12, "MAGE");
    l0TileShow(7, 5, GUY_TILE_START+8);

    message(0, startY,   "      BARB. MAGE DRUID RANGER");
    message(0, startY+1, "SPEED  *    **   **    ****");
    message(0, startY+2, "MELEE  **** *    **    ***");
    message(0, startY+3, "RANGED");
    message(0, startY+4, "   DMG **** ***  **    **");
    message(0, startY+5, "  RATE *    **** **    ****");
    message(0, startY+6, "MAGIC  *    **** ***   **");
    message(0, startY+7, "HEALTH **** *    ***   **");
    message(0, startY+8, "ARMOR  **   *    ****  **");  

    message(5, 28, "PRESS START WHEN READY");
}

unsigned char getCharacterChoice(Guy *guy, unsigned char joy) {
    unsigned char change = 0;

    if (JOY_UP(joy)) {
        guy->characterType = BARBARIAN;
        change = 1;
    } else if (JOY_LEFT(joy)) {
        guy->characterType = DRUID;
        change = 1;
    } else if (JOY_RIGHT(joy)) {
        guy->characterType = RANGER;
        change = 1;
    } else if (JOY_DOWN(joy)) {
        guy->characterType = MAGE;
        change = 1;
    }

    return change;
}

void instructions() {
    unsigned char joy, change, i;

    instructions1();

    waitForButtonPress();
    clearLayer0();
    drawOverlayBackground();
    updateOverlay();

    instructions2();

    while (1) {
        for (i=0; i<NUM_PLAYERS; i++){
            joy = joy_read(0);

            // Hold button 3 for player 2
            if ((i==0 && !JOY_BTN_3(joy)) || (i == 1 && JOY_BTN_3(joy))) {
                change = getCharacterChoice(&players[i], joy);

                if (change) {
                    waitForRelease();
                    setupPlayer(i, players[i].characterType);
                    message(30, 6+(i*9), "          "); // Clear out any previous text
                    updateCharacterTypeInOverlay(i);
                    updateOverlay();
                }
            }
            wait();
        }

        // If at least 1 player has joined the game, the game can start
        if (JOY_START(joy) && (players[0].active || players[1].active)) {
            waitForRelease();
            break;
        }
    }

    clearLayer0();
    drawOverlayBackground();
}

#pragma code-name (pop)
