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
    message(0, startY+1, "SPEED  **   **   ***   ****");
    message(0, startY+2, "MELEE  **** *    **    ***");
    message(0, startY+3, "RANGED");
    message(0, startY+4, "   DMG **** ***  **    **");
    message(0, startY+5, "  RATE *    **** **    ****");
    message(0, startY+6, "MAGIC  *    **** ***   **");
    message(0, startY+7, "HEALTH **** *    ***   **");
    message(0, startY+8, "ARMOR  **   *    ****  **");  

    message(5, 28, "PRESS A BUTTON TO START");
}

void instructions() {
    //unsigned short count = 0;
    unsigned char joy, change0;

    instructions1();

    waitForButtonPress();
    clearLayer0();
    drawOverlayBackground();
    updateOverlay();

    instructions2();

    while (1) {
        joy = joy_read(0);
        change0 = 0;

        if (JOY_UP(joy)) {
            players[0].characterType = BARBARIAN;
            change0 = 1;
        } else if (JOY_LEFT(joy)) {
            players[0].characterType = DRUID;
            change0 = 1;
        } else if (JOY_RIGHT(joy)) {
            players[0].characterType = RANGER;
            change0 = 1;
        } else if (JOY_DOWN(joy)) {
            players[0].characterType = MAGE;
            change0 = 1;
        }

        if (change0) {
            waitForRelease();
            setupPlayer(0, players[0].characterType);
            updateCharacterTypeInOverlay(0);
            updateOverlay();
        }

        if (JOY_BTN_1(joy)) {
            waitForRelease();
            break;
        }

        wait();
    }

    clearLayer0();
    drawOverlayBackground();
}
