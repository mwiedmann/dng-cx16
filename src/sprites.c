#include <cx16.h>

#include "sprites.h"
#include "config.h"
#include "utils.h"
#include "map.h"
#include "tiles.h"
#include "globals.h"

void moveAndSetAnimationFrame(unsigned short spriteAddrLo, unsigned char spriteAddrHi, unsigned char spriteGraphicLo, unsigned char spriteGraphicHi,
    unsigned short x, unsigned short y, unsigned char frame, unsigned char dir, unsigned char palOffset) {
    unsigned short addr;

    // Point to Sprite
    VERA.address = spriteAddrLo;
    VERA.address_hi = spriteAddrHi;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    addr = (spriteGraphicLo+(frame*8));

    // Graphic address bits 12:5
    VERA.data0 = addr;
    // 256 color mode, and graphic address bits 16:13
    VERA.data0 = 0b10000000 | (spriteGraphicHi + (addr > 255 ? 1 : 0));
    // Set the X and Y values
    VERA.data0 = x-scrollX;
    VERA.data0 = (x-scrollX)>>8;
    VERA.data0 = y-scrollY;
    VERA.data0 = (y-scrollY)>>8;

    VERA.data0 = 0b00001000 | dir; // Z-Depth=2 (or 0 to hide)
    VERA.data0 = 0b01010000 | palOffset;
}

void toggleSprite(unsigned short spriteAddrLo, unsigned char spriteAddrHi, unsigned short show) {
    VERA.address = spriteAddrLo+6;
    VERA.address_hi = spriteAddrHi;
    
    VERA.data0 = show ? 0b00001000 : 0;
}

void moveSprite(unsigned short spriteAddrLo, unsigned char spriteAddrHi, unsigned short x, unsigned short y) {
    // Update Sprite 1 X/Y Position
    // Point to Sprite 1 byte 2
    VERA.address = spriteAddrLo+2;
    VERA.address_hi = spriteAddrHi;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;
    
    // Set the X and Y values
    VERA.data0 = x-scrollX;
    VERA.data0 = (x-scrollX)>>8;
    VERA.data0 = y-scrollY;
    VERA.data0 = (y-scrollY)>>8;
}
