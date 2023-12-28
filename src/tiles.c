#include <cx16.h>
#include "config.h"
#include "utils.h"
#include "tiles.h"

void initTiles() {
    loadFileToVRAM("tiles.bin", TILEBASE_ADDR);
    loadFileToVRAM("tiles.pal", PALETTE_ADDR);
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
    for (i=0; i<MAPBASE_TILE_COUNT; i++) {
        VERA.data0 = 0;
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
    for (i=0; i<MAPBASE_TILE_COUNT; i++) {
        VERA.data0 = 0;
        VERA.data0 = 0;
    }
}

void drawOverlay() {
    // Note we need a `short` here because there are more than 255 tiles
    unsigned short i;

    // Clear layer 1
    VERA.address = L1_MAPBASE_ADDR;
    VERA.address_hi = L1_MAPBASE_ADDR>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Empty tiles
    for (i=0; i<MAPBASE_TILE_COUNT; i++) {
        VERA.data0 = i % 128 > 14 ? TILE_BLACK : TILE_TRANS;
        VERA.data0 = 0;
    }
}

void copyTile(unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY) {
    unsigned char tile;
    unsigned long addr = L0_MAPBASE_ADDR + (fromY*128*2) + (fromX*2);
    VERA.address = addr;
    VERA.address_hi = addr>>16;

    tile = VERA.data0;

    addr = L0_MAPBASE_ADDR + (toY*128*2) + (toX*2);
    VERA.address = addr;
    VERA.address_hi = addr>>16;

    VERA.data0 = tile;
}

void clearTile(unsigned char x, unsigned char y) {
    unsigned long addr = L0_MAPBASE_ADDR + (y*128*2) + (x*2);
    VERA.address = addr;
    VERA.address_hi = addr>>16;

    VERA.data0 = 0;
}

void clearLayers() {
    clearLayer0();
    clearLayer1();
}