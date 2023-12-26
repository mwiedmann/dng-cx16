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
    unsigned long spriteGraphicAddress = TILEBASE_ADDR + (GUY_TILE*256);

    // Point to Sprite
    VERA.address = SPRITE1_ADDR;
    VERA.address_hi = SPRITE1_ADDR>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Configure Sprite

    // Graphic address bits 12:5
    VERA.data0 = spriteGraphicAddress>>5;
    // 256 color mode, and graphic address bits 16:13
    VERA.data0 = 0b10000000 | spriteGraphicAddress>>13;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0;
    VERA.data0 = 0b00001000; // Z-Depth=2 (or 0 to hide)
    VERA.data0 = 0b01010000; // 16x16 pixel image
    
    spriteGraphicAddress = TILEBASE_ADDR + (SNAKE_TILE*256);

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

void moveAndSetAnimationFrame(unsigned char spriteId, unsigned short x, unsigned short y, short scrollX, short scrollY,
    unsigned char tileId, unsigned char frame, unsigned char dir) {
    unsigned long spriteAddr = SPRITE1_ADDR + (spriteId * 8);
    // VRAM address for sprite 1 (this is fixed)
    unsigned long spriteGraphicAddress = TILEBASE_ADDR + ((tileId+frame)*256);

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

void moveSprite(unsigned long spriteAddr, unsigned short x, unsigned short y, short scrollX, short scrollY) {
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

void moveSpriteId(unsigned char spriteId, unsigned short x, unsigned short y, short scrollX, short scrollY) {
    unsigned long spriteAddr = SPRITE1_ADDR + (spriteId * 8);

    moveSprite(spriteAddr, x, y, scrollX, scrollY);
}
