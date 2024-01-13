#include <cx16.h>
#include <joystick.h>

#include "tiles.h"
#include "globals.h"
#include "joy.h"
#include "players.h"
#include "wait.h"
#include "intro.h"

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

void showHeroes(unsigned char startY) {
    message(2, startY, "BARBARIAN");
    l0TileShow(6, startY/2, GUY_TILE_START);

    message(19, startY, "MAGE");
    l0TileShow(8, startY/2, GUY_TILE_START+5);

    message(6, startY+2, "DRUID");
    l0TileShow(6, (startY/2)+1, GUY_TILE_START+10);

    message(19, startY+2, "RANGER");
    l0TileShow(8, (startY/2)+1, GUY_TILE_START+15);
}

void instructions0() {
    message(1, 1, "WELCOME TO WARLOCK'S DUNGEON");
    message(7, 4, "MEET YOUR HEROES");

    showHeroes(6);

    message(4, 11, "FIGHT YOUR WAY THROUGH");
    message(2, 12, "ENDLESS HORDES OF MONSTERS");
    
    l0TileShow(3, 7, 96);
    l0TileShow(5, 7, 100);
    l0TileShow(7, 7, 104);
    l0TileShow(9, 7, 108);
    l0TileShow(11, 7, 112);

    l0TileShow(3, 8, 116);
    l0TileShow(5, 8, 120);
    l0TileShow(7, 8, 124);
    l0TileShow(9, 8, 128);
    l0TileShow(11, 8, 132);

    message(1, 20, "FIND THE EXIT ON EACH LEVEL");
    l0TileShow(7, 11, 32);

    message(1, 25, "CAN YOU SURVIVE AND ESCAPE?");

    message(1, 27, "USE THE JOYPAD TO READ MORE");
    message(3, 28, "PRESS A BUTTON TO BEGIN");
}

void instructions1() {
    message(6, 1, "KNOW YOUR DUNGEONS");

    message(4, 3, "USE KEYS TO OPEN DOORS");
    l0TileShow(5, 2, 38);
    l0TileShow(8, 2, 30);
    l0TileShow(9, 2, 31);

    message(2, 7, "SPEND TREASURE ON UPGRADES");
    l0TileShow(6, 4, 35);
    l0TileShow(7, 4, 36);
    l0TileShow(8, 4, 37);

    message(4, 11, "SPEED   MELEE   RANGED");
    l0TileShow(3, 6, 42);
    l0TileShow(7, 6, 43);
    l0TileShow(11, 6, 44);

    message(8, 15, "MAGIC   ARMOR");
    l0TileShow(5, 8, 45);
    l0TileShow(9, 8, 46);

    message(5, 20, "USE MAGIC SCROLLS TO");
    message(6, 21, "DAMAGE ALL ENEMIES");
    l0TileShow(7, 11, 39);

    message(1, 27, "USE THE JOYPAD TO READ MORE");
    message(3, 28, "PRESS A BUTTON TO BEGIN");
}

void showStatSheet(unsigned char startY) {
    message(0, startY,   "      BARB. MAGE DRUID RANGER");
    message(0, startY+1, "SPEED  *    **   **    ****");
    message(0, startY+2, "MELEE  **** *    **    ***");
    message(0, startY+3, "RANGED");
    message(0, startY+4, "   DMG **** ***  **    **");
    message(0, startY+5, "  RATE *    **   ***   ****");
    message(0, startY+6, "MAGIC  *    **** ****  **");
    message(0, startY+7, "HEALTH **** *    ***   **");
    message(0, startY+8, "ARMOR  **   *    ****  **");  
}

void instructions2() {
    message(7, 1, "KNOW YOUR HEROES");

    showHeroes(4);
    showStatSheet(9);

    message(1, 20, "YOUR LIFE SLOWLY TICKS AWAY");
    message(4, 22, "EAT FOOD TO REPLENISH");
    message(9, 23, "YOUR HEALTH");
    l0TileShow(6, 12, 40);
    l0TileShow(8, 12, 41);

    message(1, 27, "USE THE JOYPAD TO READ MORE");
    message(3, 28, "PRESS A BUTTON TO BEGIN");
}

void instructionsSelect() {
    message (2, 2, "PLAYERS: PRESS A DIRECTION");
    message (5, 3, "TO ACTIVATE YOUR HERO");

    message(11, 5, "BARBARIAN");
    l0TileShow(7, 3, GUY_TILE_START);

    message(5, 8, "DRUID");
    l0TileShow(5, 4, GUY_TILE_START+5);

    message(20, 8, "RANGER");
    l0TileShow(9, 4, GUY_TILE_START+10);

    message(13, 12, "MAGE");
    l0TileShow(7, 5, GUY_TILE_START+15);

    showStatSheet(15); 

    message(2, 27, "WHEN ALL PLAYERS ARE READY");
    message(6, 28, "PRESS START-RETURN");
}

void instructionsControls() {
    message(11,2, "CONTROLS");

    message(11, 5, "MOVEMENT");
    message(5, 6, "JOYPAD OR CURSOR KEYS");

    message(9, 8, "MELEE ATTACK");
    message(6, 9, "MOVE INTO MONSTER");

    message(8, 11, "RANGED ATTACK");
    message(4, 12, "KEYBOARD: X OR CTRL");
    message(4, 13, "  JOYPAD: A OR BUTTON 1");

    message(7, 15, "USE MAGIC SCROLL");
    message(4, 16, "KEYBOARD: Z OR ALT");
    message(4, 17, "  JOYPAD: B OR BUTTON 2");

    message(12, 19, "PAUSE");
    message(6, 20, "KEYBOARD: SHIFT");
    message(6, 21, "  JOYPAD: SELECT");

    message(3, 28, "PRESS A BUTTON TO BEGIN");
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
    unsigned char joy, change, i, screen=0;
    unsigned short count;

    clearVisibleLayers();
    drawOverlayBackground();
    updateOverlay();
    
    while(1)
    {
        count=0;
        switch(screen) {
            case 0: instructions0(); break;
            case 1: instructions1(); break;
            case 2: instructions2(); break;
        }

        do {
            joy = joy_read(0);
            count+= 1;
            wait();
        } while(joy == 0 && count < SCREEN_CHANGE_COUNT); // go to next screen on button press or after some time

        if (JOY_BTN_1(joy) || JOY_BTN_2(joy) || JOY_BTN_3(joy) || JOY_START(joy) || JOY_SELECT(joy)) {
            break;
        } else {
            clearVisibleLayers();
        }

        waitForRelease();

        screen += 1;
        if (screen == 3) {
            screen = 0;
        }
    }

    clearVisibleLayers();
    instructionsSelect();

    while (1) {
        for (i=0; i<NUM_PLAYERS; i++){
            joy = joy_read(0);

            // Hold button 3 for player 2
            if ((i==0 && !JOY_BTN_3(joy)) || (i == 1 && JOY_BTN_3(joy))) {
                change = getCharacterChoice(&players[i], joy);

                if (change) {
                    waitForRelease();
                    setupPlayer(i, players[i].characterType);
                    message(30, 6+(i*10), "          "); // Clear out any previous text
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

    clearVisibleLayers();
    instructionsControls();
    waitForButtonPress();

    clearLayer0();
    drawOverlayBackground();
}

#pragma code-name (pop)
