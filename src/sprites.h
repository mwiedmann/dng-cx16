#ifndef SPRITES_H
#define SPRITES_H

#define SPRITE1_ADDR 0x1FC08

void spritesConfig();
void toggleSprite(unsigned long spriteAddr, unsigned short show);
void moveSprite(unsigned long spriteAddr, unsigned short x, unsigned short y, short scrollX, short scrollY);
void moveSpriteId(unsigned char spriteId, unsigned short x, unsigned short y, short scrollX, short scrollY);

#endif