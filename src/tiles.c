#include <cx16.h>
#include "config.h"

void initTiles() {
    unsigned char x, y;
    // Point to the TileBase address so we can write to VRAM
    VERA.address = TILEBASE_ADDR;
    VERA.address_hi = TILEBASE_ADDR>>16;
    // Always set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Empty Tile
    for (y=0; y<16; y++) {
        for (x=0; x<16; x++) {
            VERA.data0 = 0;
        }
    }

    // Solid Tile
    for (y=0; y<16; y++) {
        for (x=0; x<16; x++) {
            VERA.data0 = 1;
        }
    }

    // Inner square 1
    for (y=0; y<16; y++) {
        for (x=0; x<16; x++) {
            VERA.data0 = y > 1 && y < 14 && x > 1 && x < 14
                ? 2 : 0;
        }
    }

    // Inner square 2
    for (y=0; y<16; y++) {
        for (x=0; x<16; x++) {
            VERA.data0 = y > 1 && y < 14 && x > 1 && x < 14
                ? 3 : 0;
        }
    }
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

void clearLayers() {
    clearLayer0();
    clearLayer1();
}