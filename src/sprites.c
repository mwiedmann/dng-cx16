#include <cx16.h>

#include "sprites.h"
#include "config.h"
#include "utils.h"
#include "map.h"
#include "tiles.h"
#include "globals.h"

void spritesConfig() {
    unsigned char i;

    // VRAM address for sprite 1 (this is fixed)
    unsigned long spriteGraphicAddress = L0_TILEBASE_ADDR + (GUY_TILE_START*L0_TILE_SIZE);

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
    spriteGraphicAddress = L0_TILEBASE_ADDR + (AXE_TILE*L0_TILE_SIZE);

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

void moveAndSetAnimationFrame(unsigned char spriteId, unsigned short x, unsigned short y, unsigned char tileId, unsigned char frame, unsigned char dir) {
    unsigned long spriteAddr = SPRITE_ADDR_START + (spriteId * 8);
    // VRAM address for sprite 1 (this is fixed)
    unsigned long spriteGraphicAddress = L0_TILEBASE_ADDR + ((tileId+frame)*L0_TILE_SIZE);

    // Point to Sprite
    VERA.address = spriteAddr;
    VERA.address_hi = spriteAddr>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Graphic address bits 12:5
    VERA.data0 = spriteGraphicAddress>>5;
    // 256 color mode, and graphic address bits 16:13
    VERA.data0 = 0b10000000 | spriteGraphicAddress>>13;
    // Set the X and Y values
    VERA.data0 = x-scrollX;
    VERA.data0 = (x-scrollX)>>8;
    VERA.data0 = y-scrollY;
    VERA.data0 = (y-scrollY)>>8;

    VERA.data0 = 0b00001000 | dir; // Z-Depth=2 (or 0 to hide)
}

void toggleSprite(unsigned long spriteAddr, unsigned short show) {
    VERA.address = spriteAddr+6;
    VERA.address_hi = spriteAddr>>16;
    
    VERA.data0 = show ? 0b00001000 : 0;
}

void toggleWeapon(unsigned char playerId, unsigned short show) {
    toggleSprite(playerId == 0 ? WEAPON_1_SPRITE_ADDR : WEAPON_2_SPRITE_ADDR, show);
}

void moveSprite(unsigned long spriteAddr, unsigned short x, unsigned short y) {
    // Update Sprite 1 X/Y Position
    // Point to Sprite 1 byte 2
    VERA.address = spriteAddr+2;
    VERA.address_hi = spriteAddr>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;
    
    // Set the X and Y values
    VERA.data0 = x-scrollX;
    VERA.data0 = (x-scrollX)>>8;
    VERA.data0 = y-scrollY;
    VERA.data0 = (y-scrollY)>>8;
}

void moveSpriteId(unsigned char spriteId, unsigned short x, unsigned short y) {
    unsigned long spriteAddr = SPRITE_ADDR_START + (spriteId * 8);

    moveSprite(spriteAddr, x, y);
}
