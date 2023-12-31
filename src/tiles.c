#include <stdio.h>
#include <cx16.h>
#include "config.h"
#include "utils.h"
#include "tiles.h"
#include "globals.h"

void initTiles() {
    loadFileToVRAM("l0tiles.bin", L0_TILEBASE_ADDR);
    loadFileToVRAM("l1tiles.bin", L1_TILEBASE_ADDR);
    loadFileToVRAM("tiles.pal", PALETTE_ADDR);
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
    } else {
        return 47;
    }

    return tile;
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

void clearLayer1() {
    // Note we need a `short` here because there are more than 255 tiles
    unsigned short i;

    // Clear layer 1
    VERA.address = L1_MAPBASE_ADDR;
    VERA.address_hi = L1_MAPBASE_ADDR>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Empty tiles
    for (i=0; i<L1_MAPBASE_TILE_COUNT; i++) {
        VERA.data0 = L1_TILE_TRANS;
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
        addr = L1_MAPBASE_ADDR + ((6+(i*9))*L1_MAPBASE_TILE_WIDTH*2) + ((L1_OVERLAY_X+1)*2);
        VERA.address = addr;
        VERA.address_hi = addr>>16;
        // Always set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;

        for (x=0; x<8; x++) {
            VERA.data0 = (CHARACTER_NAME_TILES+(players[i].characterType*16))+x+(i*8);
            VERA.data0 = 0;
        }

        // Show SCORE and GOLD
        addr = L1_MAPBASE_ADDR + ((7+(i*9))*L1_MAPBASE_TILE_WIDTH*2) + ((L1_OVERLAY_X)*2);
        VERA.address = addr;
        VERA.address_hi = addr>>16;
        // Always set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;

        for (x=0; x<5; x++) {
            VERA.data0 = 240+x+(i*8);
            VERA.data0 = 0;
        }

        VERA.data0 = L1_TILE_BLACK;
        VERA.data0 = 0;

        for (x=0; x<4; x++) {
            VERA.data0 = x+(i*8);
            VERA.data0 = 1;
        }

        // Show HEALTH
        addr = L1_MAPBASE_ADDR + ((9+(i*9))*L1_MAPBASE_TILE_WIDTH*2) + ((L1_OVERLAY_X)*2);
        VERA.address = addr;
        VERA.address_hi = addr>>16;
        // Always set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;

        for (x=0; x<6; x++) {
            VERA.data0 = 16+x+(i*8);
            VERA.data0 = 1;
        }
    }
}

void message(unsigned char x, unsigned char y, char *msg) {
    unsigned char i=0;
    unsigned long addr = L1_MAPBASE_ADDR + (y*L1_MAPBASE_TILE_WIDTH*2) + (x*2);

    VERA.address = addr;
    VERA.address_hi = addr>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    do {
        VERA.data0 = letterToTile(msg[i]);
        VERA.data0 = 0;
        i++;
    } while(msg[i] != 0 || i > 10);

}

void updateOverlay() {
    char buf[16];

    sprintf(buf, "%05u %04u", players[0].score, players[0].gold);
    message(30, 8, buf);

    sprintf(buf, "%05u", players[0].health);
    message(30, 10, buf);

    sprintf(buf, "%05u %04u", players[1].score, players[1].gold);
    message(30, 17, buf);

    sprintf(buf, "%05u", players[1].health);
    message(30, 19, buf);
}

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

void clearLayers() {
    clearLayer0();
    clearLayer1();
}