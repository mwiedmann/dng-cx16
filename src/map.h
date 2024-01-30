#ifndef MAP_H
#define MAP_H

void createEntity(unsigned char tile, unsigned char entityId, unsigned char x, unsigned char y, unsigned char isShot);
void createMapStatus(unsigned char level);
void drawMap(unsigned char level);

#endif
