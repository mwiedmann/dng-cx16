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
    BANK_NUM = CODE_BANK;
    cbm_k_setnam("dng.prg.02");
    cbm_k_setlfs(0, 8, 0); // Skip the first 2 bytes of the file. They just hold the size in bytes.
    cbm_k_load(0, (unsigned short)BANK_RAM);
}


#pragma code-name (push, "BANKRAM02")

void init() {
    // Configure the joysticks
    joy_install(cx16_std_joy);

    // Enable both layers
    VERA.display.video = 0b11110001;

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
    VERA.display.hscale = 64;
    VERA.display.vscale = 64;
}

void initTiles() {
    loadFileToVRAM("gmtiles.bin", L0_TILEBASE_GAME_ADDR);
    loadFileToVRAM("ovtiles.bin", L1_TILEBASE_ADDR);
    loadFileToVRAM("tiles.pal", PALETTE_ADDR);
}

void spritesConfig() {
    unsigned char i;

    // VRAM address for sprite 1 (this is fixed)
    unsigned long spriteGraphicAddress = L0_TILEBASE_ADDR + (GUY_TILE_START*L0_TILE_SIZE); // This will change

    // Point to Sprite
    VERA.address = GUY_1_SPRITE_ADDR;
    VERA.address_hi = GUY_1_SPRITE_ADDR>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Configure Guy 1

    // Graphic address bits 12:5
    VERA.data0 = spriteGraphicAddress>>5;
    // 256 color mode, and graphic address bits 16:13
    VERA.data0 = 0b10000000 | spriteGraphicAddress>>13;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0b00000000; // Z-Depth=2 (or 0 to hide)
    VERA.data0 = 0b01010000; // 16x16 pixel image

    // Guy 2
    spriteGraphicAddress = L0_TILEBASE_ADDR + ((GUY_TILE_START+32)*L0_TILE_SIZE);
    // Graphic address bits 12:5
    VERA.data0 = spriteGraphicAddress>>5;
    // 256 color mode, and graphic address bits 16:13
    VERA.data0 = 0b10000000 | spriteGraphicAddress>>13;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0b00000000; // Z-Depth=2 (or 0 to hide)
    VERA.data0 = 0b01010000; // 16x16 pixel image

    // Configure Weapon 1
    spriteGraphicAddress = L0_TILEBASE_ADDR + (GUY_TILE_START*L0_TILE_SIZE); // This will change

    // Graphic address bits 12:5
    VERA.data0 = spriteGraphicAddress>>5;
    // 256 color mode, and graphic address bits 16:13
    VERA.data0 = 0b10000000 | spriteGraphicAddress>>13;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0b00000000; // Z-Depth=2 (or 0 to hide)
    VERA.data0 = 0b01010000; // 16x16 pixel image


    // Weapon 2
    VERA.data0 = spriteGraphicAddress>>5;
    // 256 color mode, and graphic address bits 16:13
    VERA.data0 = 0b10000000 | spriteGraphicAddress>>13;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0b00000000; // Z-Depth=2 (or 0 to hide)
    VERA.data0 = 0b01010000; // 16x16 pixel image
    
    spriteGraphicAddress = L0_TILEBASE_ADDR + (MONSTER_TILE*L0_TILE_SIZE);

    for (i=0; i<ENTITY_COUNT; i++) {
        VERA.data0 = spriteGraphicAddress>>5;
        // 256 color mode, and graphic address bits 16:13
        VERA.data0 = 0b10000000 | spriteGraphicAddress>>13;
        VERA.data0 = 0;
        VERA.data0 = 0;
        VERA.data0 = 0;
        VERA.data0 = 0;
        VERA.data0 = 0b00000000; // Z-Depth=2 (or 0 to hide)
        VERA.data0 = 0b01010000; // 16x16 pixel image
    }
}

#pragma code-name (pop)
