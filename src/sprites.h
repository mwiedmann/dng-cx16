#ifndef SPRITES_H
#define SPRITES_H

#define SPRITE_ADDR_START 0x1FC08

#define GUY_1_SPRITE_ADDR SPRITE_ADDR_START
#define GUY_2_SPRITE_ADDR GUY_1_SPRITE_ADDR + 8L

#define WEAPON_1_SPRITE_ADDR GUY_2_SPRITE_ADDR + 8L
#define WEAPON_2_SPRITE_ADDR WEAPON_1_SPRITE_ADDR + 8L

#define ENTITY_SPRITE_ADDR WEAPON_2_SPRITE_ADDR + 8L

#define WEAPON_SPRITE_ID_START 2

#define ANIMATION_FRAME_SPEED 4
#define ANIMATION_FRAME_COUNT 3

void moveAndSetAnimationFrame(unsigned short spriteAddrLo, unsigned char spriteAddrHi, unsigned char spriteGraphicLo, unsigned char spriteGraphicHi,
    unsigned short x, unsigned short y, unsigned char frame, unsigned char dir, unsigned char palOffset);
void toggleSprite(unsigned short spriteAddrLo, unsigned char spriteAddrHi, unsigned short show);
void moveSprite(unsigned short spriteAddrLo, unsigned char spriteAddrHi, unsigned short x, unsigned short y);

#endif