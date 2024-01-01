#ifndef SPRITES_H
#define SPRITES_H

#define SPRITE_ADDR_START 0x1FC08

#define GUY_1_SPRITE_ADDR SPRITE_ADDR_START
#define GUY_2_SPRITE_ADDR GUY_1_SPRITE_ADDR + 8L

#define WEAPON_1_SPRITE_ADDR GUY_2_SPRITE_ADDR + 8L
#define WEAPON_2_SPRITE_ADDR WEAPON_1_SPRITE_ADDR + 8L

#define ENTITY_SPRITE_ADDR WEAPON_2_SPRITE_ADDR + 8L

#define WEAPON_SPRITE_ID_START 2

#define AXE_TILE 69
#define GUY_TILE_START 64

#define MONSTER_TILE 128

#define GENERATOR_TILE 48

#define ANIMATION_FRAME_SPEED 4
#define ANIMATION_FRAME_COUNT 3

void spritesConfig();
void moveAndSetAnimationFrame(unsigned char spriteId, unsigned short x, unsigned short y, short scrollX, short scrollY,
    unsigned char tileId, unsigned char frame, unsigned char dir);
void toggleSprite(unsigned long spriteAddr, unsigned short show);
void toggleWeapon(unsigned char playerId, unsigned short show);
void moveSprite(unsigned long spriteAddr, unsigned short x, unsigned short y, short scrollX, short scrollY);
void moveSpriteId(unsigned char spriteId, unsigned short x, unsigned short y, short scrollX, short scrollY);

#endif