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

    message(8, 7, "SHOOT ENEMIES");
    message(7, 8, "OR MELEE ATTACK");

    l0TileShow(0, 8, 46);
    l0TileShow(1, 8, 45);
    l0TileShow(2, 8, 44);
    message(7, 16, "GET TREASURE FOR GOLD");
    message(7, 17, "SPEND ON UPGRADES!");

    l0TileShow(1, 10, 49);
    message(5, 20, "SCROLLS HIT ALL ENEMIES");

    l0TileShow(1, 11, 48);
    message(5, 22, "KEYS OPEN DOORS:");
    l0TileShow(11, 11, 40);
    l0TileShow(12, 11, 41);
    l0TileShow(13, 11, 42);

    l0TileShow(1, 12, 50);
    l0TileShow(2, 12, 51);
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

    message(5, 28, "PRESS A BUTTON TO START");
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
                    updateCharacterTypeInOverlay(i);
                    updateOverlay();
                }
            }
            wait();
        }

        if (JOY_BTN_1(joy)) {
            waitForRelease();
            break;
        }
    }

    clearLayer0();
    drawOverlayBackground();
}
