#include <stdio.h>
#include <cx16.h>
#include <joystick.h>

#include "tiles.h"
#include "globals.h"
#include "joy.h"
#include "players.h"
#include "wait.h"
#include "intro.h"
#include "strtbl.h"

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

void showMessageList(MessageList *list, unsigned char length, unsigned char stringStart) {
    unsigned i;

    for (i=0; i<length; i++) {
        message(list[i].x, list[i].y, getString(stringStart+i));
    }
}

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
    MessageList list[INSTR0_LENGTH] = {
        { 1, 1 },
        { 7, 4 },
        { 4, 11 },
        { 2, 12 },
        { 1, 20 },
        { 1, 25 },
        { 1, 27 },
        { 3, 28 }
    };

    showMessageList(list, INSTR0_LENGTH, STRING_INSTR0_START);
    
    showHeroes(6);

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

    l0TileShow(7, 11, 32);
}

void instructions1() {
    MessageList list[INSTR1_LENGTH] = {
        { 6, 1 },
        { 4, 3 },
        { 2, 7 },
        { 4, 11 },
        { 8, 15 },
        { 5, 20 },
        { 6, 21 },
        { 1, 27 },
        { 3, 28 }
    };

    showMessageList(list, INSTR1_LENGTH, STRING_INSTR1_START);

    l0TileShow(5, 2, 38);
    l0TileShow(8, 2, 30);
    l0TileShow(9, 2, 31);

    l0TileShow(6, 4, 35);
    l0TileShow(7, 4, 36);
    l0TileShow(8, 4, 37);

    l0TileShow(3, 6, 42);
    l0TileShow(7, 6, 43);
    l0TileShow(11, 6, 44);

    l0TileShow(5, 8, 45);
    l0TileShow(9, 8, 46);

    l0TileShow(7, 11, 39);
}

void showStatSheet(unsigned char startY) {
    unsigned char i;

    for (i=0; i<STAT_SHEET_LENGTH; i++) {
        message(0, startY+i, getString(STRING_STAT_SHEET_START+i));
    }
}

void instructions2() {
    MessageList list[INSTR2_LENGTH] = {
        { 7, 1 },
        { 1, 20 },
        { 4, 22 },
        { 9, 23 },
        { 1, 27 },
        { 3, 28 }
    };

    showMessageList(list, INSTR2_LENGTH, STRING_INSTR2_START);

    showHeroes(4);
    showStatSheet(9);

    l0TileShow(6, 12, 40);
    l0TileShow(8, 12, 41);
}

void instructionsSelect() {
    MessageList list[INSTR_SELECT_LENGTH] = {
        { 2, 2 },
        { 5, 3 },
        { 11, 5 },
        { 5, 8 },
        { 20, 8 },
        { 13, 12 },
        { 2, 27 },
        { 6, 28 }
    };

    showMessageList(list, INSTR_SELECT_LENGTH, STRING_INSTR_SELECT_START);

    l0TileShow(7, 3, GUY_TILE_START);
    l0TileShow(5, 4, GUY_TILE_START+5);
    l0TileShow(9, 4, GUY_TILE_START+10);
    l0TileShow(7, 5, GUY_TILE_START+15);

    showStatSheet(15); 
}

void instructionsControls() {
    MessageList list[INSTR_CONTROLS_LENGTH] = {
        { 11,2 },
        { 11, 5 },
        { 5, 6 },
        { 9, 8 },
        { 6, 9 },
        { 8, 11 },
        { 4, 12 },
        { 4, 13 },
        { 7, 15 },
        { 4, 16 },
        { 4, 17 },
        { 12, 19 },
        { 6, 20 },
        { 6, 21 },
        { 3, 28 }
    };

    showMessageList(list, INSTR_CONTROLS_LENGTH, STRING_INSTR_CONTROLS_START);
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

void showLevelIntro() {
    char *msg[4] = {0,0,0,0};
    char buf[40];

    clearVisibleLayers();

    sprintf(buf, "LEVEL %u", level);
    msg[0] = buf;
    msg[2] = isShopLevel ? "PURCHASE UPGRADES" : "CAN YOU SURVIVE?";
    msg[3] = isShopLevel ? "AND REST FOR A MOMENT" : "I DOUBT IT!";

    messageCenter(msg);

    waitCount(240);
    clearLayer1VisibleArea();
}

#pragma code-name (pop)
