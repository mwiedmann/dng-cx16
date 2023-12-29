#ifndef SPRITES_H
#define SPRITES_H

#define SPRITE1_ADDR 0x1FC08
#define WEAPON_SPRITE_ADDR SPRITE1_ADDR + 8L
#define AXE_TILE 69
#define GUY_TILE 64
#define SKELETON_TILE 128
#define SLIME_TILE 132
#define SNAKE_TILE 136
#define DRAGON_TILE 140
#define SPIDER_TILE 144
#define GOLEM_TILE 148
#define BAT_TILE 152
#define BEHOLDER_TILE 156
#define DEMON_TILE 160
#define INSECTS_TILE 164

#define GENERATOR_TILE 48

#define ANIMATION_FRAME_SPEED 4
#define ANIMATION_FRAME_COUNT 3

void spritesConfig();
void moveAndSetAnimationFrame(unsigned char spriteId, unsigned short x, unsigned short y, short scrollX, short scrollY,
    unsigned char tileId, unsigned char frame, unsigned char dir);
void toggleSprite(unsigned long spriteAddr, unsigned short show);
void toggleWeapon(unsigned short show);
void moveSprite(unsigned long spriteAddr, unsigned short x, unsigned short y, short scrollX, short scrollY);
void moveSpriteId(unsigned char spriteId, unsigned short x, unsigned short y, short scrollX, short scrollY);

#endif