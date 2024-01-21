#include <stdio.h>
#include <string.h>
#include <cx16.h>
#include <joystick.h>

#include "config.h"
#include "utils.h"
#include "tiles.h"
#include "globals.h"
#include "wait.h"
#include "joy.h"
#include "players.h"
#include "sound.h"

#pragma code-name (push, "BANKRAM02")

void loadDungeonTiles() {
    char buf[16];
    unsigned char dngType = level % 5 == 0 ? 0 : (level % 5) - 1;

    sprintf(buf, "d%utiles.bin", dngType);
    loadFileToVRAM(buf, L0_TILEBASE_DNG_ADDR);
}

void clearLayer0() {
    // Note we need a `short` here because there are more than 255 tiles
    unsigned short i;
    
    // Clear layer 0
    VERA.address = L0_MAPBASE_ADDR;
    VERA.address_hi = L0_MAPBASE_ADDR>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Empty tiles
    for (i=0; i<L0_MAPBASE_TILE_COUNT; i++) {
        VERA.data0 = TILE_TRANS;
        VERA.data0 = 0;
    }
}

void clearLayer1VisibleArea() {
    // Note we need a `short` here because there are more than 255 tiles
    unsigned short x, y;

    // Empty tiles
    for (y=0; y<30; y++) {
        VERA.address = L1_MAPBASE_ADDR + (y*L1_MAPBASE_TILE_WIDTH*2);
        VERA.address_hi = (L1_MAPBASE_ADDR + (y*L1_MAPBASE_TILE_WIDTH*2))>>16;
        // Always set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;

        for (x=0; x<30; x++) {
            VERA.data0 = L1_TILE_TRANS;
            VERA.data0 = 0;
        }
    }
}

void clearLayer0VisibleArea() {
    // Note we need a `short` here because there are more than 255 tiles
    unsigned short x, y;

    // Empty tiles
    for (y=0; y<15; y++) {
        VERA.address = L0_MAPBASE_ADDR + (y*L0_MAPBASE_TILE_WIDTH*2);
        VERA.address_hi = (L0_MAPBASE_ADDR + (y*L0_MAPBASE_TILE_WIDTH*2))>>16;
        // Always set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;
        
        for (x=0; x<15; x++) {
            VERA.data0 = TILE_TRANS;
            VERA.data0 = 0;
        }
    }
}

void clearVisibleLayers() {
    clearLayer0VisibleArea();
    clearLayer1VisibleArea();
}

void updateCharacterTypeInOverlay(unsigned char playerId) {
    unsigned char x;
    unsigned long addr;

    addr = L1_MAPBASE_ADDR + ((6+(playerId*10))*L1_MAPBASE_TILE_WIDTH*2) + ((L1_OVERLAY_X+1)*2);
    VERA.address = addr;
    VERA.address_hi = addr>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    for (x=0; x<8; x++) {
        VERA.data0 = (CHARACTER_NAME_TILES+(players[playerId].characterType*16))+x+(playerId*8);
        VERA.data0 = 0;
    }
}

void drawOverlayBackground() {
    // Note we need a `short` here because there are more than 255 tiles
    unsigned char y, x, tile=WARLOCKS_DUNGEON_TILE, i;
    unsigned long addr;

    // Clear layer 1
    VERA.address = L1_MAPBASE_ADDR;
    VERA.address_hi = L1_MAPBASE_ADDR>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Trans and Black background
    for (y=0; y<L1_MAPBASE_TILE_HEIGHT; y++) {
        for (x=0; x<L1_MAPBASE_TILE_WIDTH; x++) {
            VERA.data0 = x % L1_MAPBASE_TILE_WIDTH >= L1_OVERLAY_X
                ? L1_TILE_BLACK
                : L1_TILE_TRANS;
            VERA.data0 = 0;
        }
    }

    // Draw Warlock's Dungeon
    for (y=0; y<5; y++) {
        addr = L1_MAPBASE_ADDR + (y*L1_MAPBASE_TILE_WIDTH*2) + (L1_OVERLAY_X*2);
        VERA.address = addr;
        VERA.address_hi = addr>>16;
        // Always set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;

        for (x=0; x<10; x++) {
            VERA.data0 = tile;
            VERA.data0 = 0;
            tile++;
        }

        tile+=6;
    }

    for (i=0; i<NUM_PLAYERS; i++) {
        if (!players[i].active) {
            message(30, 6+(i*10), "JOIN GAME!");
        } else {
            message(30, 6+(i*10), "          "); // Clear out any previous text
            updateCharacterTypeInOverlay(i);
        }

        // Show SCORE and GOLD
        addr = L1_MAPBASE_ADDR + ((7+(i*10))*L1_MAPBASE_TILE_WIDTH*2) + ((L1_OVERLAY_X)*2);
        VERA.address = addr;
        VERA.address_hi = addr>>16;
        // Always set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;

        for (x=0; x<5; x++) {
            VERA.data0 = 208+x+(i*8);
            VERA.data0 = 0;
        }

        VERA.data0 = L1_TILE_BLACK;
        VERA.data0 = 0;

        for (x=0; x<4; x++) {
            VERA.data0 = 224+x+(i*8);
            VERA.data0 = 0;
        }

        // Show HEALTH
        addr = L1_MAPBASE_ADDR + ((9+(i*10))*L1_MAPBASE_TILE_WIDTH*2) + ((L1_OVERLAY_X)*2);
        VERA.address = addr;
        VERA.address_hi = addr>>16;
        // Always set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;

        for (x=0; x<6; x++) {
            VERA.data0 = 240+x+(i*8);
            VERA.data0 = 0;
        }
    }
}

void l0TileShow(unsigned char x, unsigned char y, unsigned char tile) {
    unsigned long addr = L0_MAPBASE_ADDR + (y*L0_MAPBASE_TILE_WIDTH*2) + (x*2);

    VERA.address = addr;
    VERA.address_hi = addr>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    VERA.data0 = tile;
    VERA.data0 = 0;
}


void flashLayer1() {
     // Note we need a `short` here because there are more than 255 tiles
    unsigned char y, x, i;
    unsigned long addr;

    
    for (i=0; i<2; i++) {
        for (y=0; y<L1_OVERLAY_X; y++) {
            addr = L1_MAPBASE_ADDR + (y*L1_MAPBASE_TILE_WIDTH*2);
        
            VERA.address = addr;
            VERA.address_hi = addr>>16;
            // Always set the Increment Mode, turn on bit 4
            VERA.address_hi |= 0b10000;

            for (x=0; x<L1_OVERLAY_X; x++) {
                VERA.data0 = i == 0 ? L1_TILE_WHITE : L1_TILE_TRANS;
                VERA.data0 = 0;
            }
        }

        if (i==0) {
            waitCount(10);
        }
    }
}

void clearL1PlayArea() {
    unsigned char y, x, i;
    unsigned long addr;

    for (i=0; i<2; i++) {
        for (y=0; y<L1_OVERLAY_X; y++) {
            addr = L1_MAPBASE_ADDR + (y*L1_MAPBASE_TILE_WIDTH*2);
        
            VERA.address = addr;
            VERA.address_hi = addr>>16;
            // Always set the Increment Mode, turn on bit 4
            VERA.address_hi |= 0b10000;

            for (x=0; x<L1_OVERLAY_X; x++) {
                VERA.data0 = L1_TILE_TRANS;
                VERA.data0 = 0;
            }
        }
    }
}

unsigned char shopQuestion(unsigned short cost, char *item) {
    char buf[30];
    char * msg[4] = {0,0,"PRESS BUTTON TO ACCEPT","START TO CANCEL"};
    unsigned char pressed;

    msg[0] = item;

    sprintf(buf, "PRICE: %u GOLD", cost);
    msg[1] = buf;

    messageCenter(msg);

    pressed = waitForButtonPress();

    clearL1PlayArea();

    // We don't check controls every frame so we need to cancel the move
    // or the player repeated touches the item
    stopMove(0);
    stopMove(1);

    return !JOY_START(pressed);
}

void gameMessage(char *msg1, char *msg2, unsigned char sound) {
    char * msg[4] = {0,0,0,0};

    msg[0] = msg1;
    msg[1] = msg2;

    if (sound) {
        soundPlayMusic(sound);
    }

    messageCenter(msg);
    waitCount(150);

    clearL1PlayArea();
}

void updateOverlay() {
    unsigned char i,p;
    char buf[16];

    for (p=0; p<NUM_PLAYERS; p++) {
        sprintf(buf, "% 5u % 4u", players[p].score, players[p].gold);
        message(30, 8+(p*10), buf);

        sprintf(buf, "% 5u", players[p].health);
        message(30, 10+(p*10), buf);

        // Set the buffer to all spaces to start
        for (i=0; i<INVENTORY_LIMIT; i++) {
            buf[i] = ' ';
        }

        for (i=0; i<players[p].keys; i++) {
            buf[i]=160;
        }

        for (i=0; i<players[p].scrolls; i++) {
            buf[(INVENTORY_LIMIT-1)-i]=161;
        }

        buf[10] = 0;
        message(30, 11+(p*10), buf);

        for (i=0; i<5; i++) {
            buf[i] = players[p].hasBoosts[i] ? 162+i : ' ';
        }

        buf[5] = 0;
        message(30, 12+(p*10), buf);
    }
}

void message(unsigned char x, unsigned char y, char *msg) {
    unsigned char i=0;
    unsigned long addr = L1_MAPBASE_ADDR + (y*L1_MAPBASE_TILE_WIDTH*2) + (x*2);

    VERA.address = addr;
    VERA.address_hi = addr>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    while(msg[i] != 0 || i > 30) {
        VERA.data0 = letterToTile(msg[i]);
        VERA.data0 = 0;
        i++;
    };
}

void messageCenter(char *msg[4]) {
    unsigned char len, col, i;

    for (i=0; i<4; i++) {
        if (msg[i]) {
            len = strlen(msg[i]);
            col = 15 - (len>>1);
            message(col, 12+i, msg[i]);
        }
    }
}

 unsigned char letterToTile(char letter) {
    unsigned char tile;

    if (letter >= 193 && letter <= 218) {
        tile = letter - 193;
    } else if (letter >= 48 && letter <= 57) {
        tile = letter - 22;
    } else if (letter == 58) {
        tile = 36;
    } else if (letter == 46) {
        tile = 37;
    } else if (letter == 63) {
        tile = 38;
    } else if (letter == 45) {
        tile = 39;
    } else if (letter == 33) {
        tile = 40;
    } else if (letter == 39) {
        tile = 41;
    } else if (letter == 42) {
        tile = 42;
    } else if (letter >= 160 && letter <= 166) { // Key through upgrades
        tile = (letter - 100) - 14; // <-- Had to adjust tile location a few times...just adjust here now
    } else {
        return 44;
    }

    return tile;
}

#pragma code-name (pop)

void copyTile(unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY) {
    unsigned char tile;
    unsigned long addr = L0_MAPBASE_ADDR + (fromY*L0_MAPBASE_TILE_WIDTH*2) + (fromX*2);
    VERA.address = addr;
    VERA.address_hi = addr>>16;

    tile = VERA.data0;

    addr = L0_MAPBASE_ADDR + (toY*L0_MAPBASE_TILE_WIDTH*2) + (toX*2);
    VERA.address = addr;
    VERA.address_hi = addr>>16;

    VERA.data0 = tile;
}
