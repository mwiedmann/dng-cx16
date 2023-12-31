#include <cx16.h>
#include <joystick.h>
#include <stdio.h>

#include "config.h"
#include "utils.h"

void init() {
    // Load some hi-ram code
    BANK_NUM = INSTRUCTIONS_BANK;
    cbm_k_setnam("dng.prg.02");
    cbm_k_setlfs(0, 8, 0); // Skip the first 2 bytes of the file. They just hold the size in bytes.
    cbm_k_load(0, (unsigned short)BANK_RAM);

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
