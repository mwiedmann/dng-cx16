#include <cx16.h>
#include <joystick.h>
#include <stdio.h>

#include "globals.h"
#include "tiles.h"
#include "config.h"
#include "utils.h"
#include "sprites.h"

void loadBankedCode() {
    // Load some hi-ram code

    // We load a little code into BANK 1 that deals with the entityLists
    RAM_BANK = MAP_BANK;
    cbm_k_setnam("dng.prg.01");
    cbm_k_setlfs(0, 8, 0); // Skip the first 2 bytes of the file. They just hold the size in bytes.
    cbm_k_load(0, (unsigned short)BANK_RAM);

    RAM_BANK = CODE_BANK;
    cbm_k_setnam("dng.prg.02");
    cbm_k_setlfs(0, 8, 0); // Skip the first 2 bytes of the file. They just hold the size in bytes.
    cbm_k_load(0, (unsigned short)BANK_RAM);

    RAM_BANK = CONFIG_BANK;
    cbm_k_setnam("dng.prg.03");
    cbm_k_setlfs(0, 8, 0); // Skip the first 2 bytes of the file. They just hold the size in bytes.
    cbm_k_load(0, (unsigned short)BANK_RAM);
}

void toggleLayers(unsigned char show) {
    VERA.display.video = show 
        ? VERA.display.video | 0b11110000
        : (VERA.display.video | 0b11000000) & 0b11001111;
}

#pragma code-name (push, "BANKRAM03")

void showTitle() {
    unsigned char compositeMode = VERA.display.video & 0b10;

    // Configure the joysticks
    joy_install(cx16_std_joy);

#ifndef TEST_MODE

    // Bitmap mode
    // Color Depth 3 - 8 bpp
    VERA.layer1.config = 0b00000111;

    // Get bytes 16-11 of the new TileBase address
    // Set bit 0 to 0 (for 320 mode), its already 0 in this address
    VERA.layer1.tilebase = 0;

    // Need these to get true 320x240
    // otherwise VERA will duplicate the screen and show extra (random) bytes from VRAM
    VERA.display.hscale = hscale;
    VERA.display.vscale = vscale;

    cbm_k_setnam(compositeMode ? "titlecmp.pal" : "title.pal");
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(3, PALETTE_ADDR);

    cbm_k_setnam(compositeMode ? "titlecmp.bin" : "title.bin");
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(2, 0);

    // Enable both layers
    VERA.display.video |= 0b11110000;
#endif
}

void init() {
    // With 16 pixel tiles, we don't need as many tiles (might need more later for scrolling)
    // Only 640/16 = 40, 480/16=30 (40x30 tile resolution now)
    // Set the Map Height=00 (32), Width=00 (32)
    // Set Color Depth to 8 bpp mode
    VERA.layer0.config = 0b00000011;
    VERA.layer1.config = 0b00010011;
    
    // Get bytes 16-9 of the MapBase addresses and set on both layers
    VERA.layer0.mapbase = L0_MAPBASE_ADDR>>9;
    VERA.layer1.mapbase = L1_MAPBASE_ADDR>>9;

    // Layers can share a tilebase (use the same tiles)
    // Get bytes 16-11 of the new TileBase address
    // ALSO Set Tile W/H (bits 0/1) to 1 for 16 pixel tiles
    VERA.layer0.tilebase = L0_TILEBASE_ADDR>>9 | 0b11;
    VERA.layer1.tilebase = L1_TILEBASE_ADDR>>9;

    // Set 320x240 mode
    VERA.display.hscale = hscale;
    VERA.display.vscale = vscale;
}

void initTiles() {
    cbm_k_setnam("tiles.pal");
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(3, PALETTE_ADDR);

    cbm_k_setnam("gmtiles.bin");
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(2, L0_TILEBASE_GAME_ADDR);

    cbm_k_setnam("ovtiles.bin");
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(2, L1_TILEBASE_ADDR);
}

void spritesConfig() {
    unsigned char i;

    VERA.address = GUY_1_SPRITE_ADDR;
    VERA.address_hi = GUY_1_SPRITE_ADDR>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    for (i=0; i<ENTITY_COUNT+4; i++) {
    // Graphic address bits 12:5
        VERA.data0 = 0;
        VERA.data0 = 0b10000000;
        VERA.data0 = 0;
        VERA.data0 = 0;
        VERA.data0 = 0;
        VERA.data0 = 0;
        VERA.data0 = 0b00000000; // Z-Depth=2 (or 0 to hide)
        VERA.data0 = 0b01010000; // 16x16 pixel image
    }
    
    // Configure Player 1 and 2
    players[0].spriteAddrLo = GUY_1_SPRITE_ADDR;
    players[0].spriteAddrHi = GUY_1_SPRITE_ADDR>>16;
    players[1].spriteAddrLo = GUY_2_SPRITE_ADDR;
    players[1].spriteAddrHi = GUY_2_SPRITE_ADDR>>16;
    weapons[0].spriteAddrLo = WEAPON_1_SPRITE_ADDR;
    weapons[0].spriteAddrHi = WEAPON_1_SPRITE_ADDR>>16;
    weapons[1].spriteAddrLo = WEAPON_2_SPRITE_ADDR;
    weapons[1].spriteAddrHi = WEAPON_2_SPRITE_ADDR>>16;
}

#pragma code-name (pop)
