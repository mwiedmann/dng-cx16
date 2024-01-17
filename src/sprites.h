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

void spritesConfig();
void moveAndSetAnimationFrame(unsigned char spriteId, unsigned short x, unsigned short y,
    unsigned char tileId, unsigned char frame, unsigned char dir, unsigned char palOffset);
void toggleSprite(unsigned long spriteAddr, unsigned short show);
void toggleWeapon(unsigned char playerId, unsigned short show);
void moveSprite(unsigned long spriteAddr, unsigned short x, unsigned short y);
void moveSpriteId(unsigned char spriteId, unsigned short x, unsigned short y);

#endif