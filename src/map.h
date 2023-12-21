#ifndef MAP_H
#define MAP_H

#define MAP_MAX 90
#define GUY_MAX (MAP_MAX*16)-(16+GUY_SPEED) // Right edge - Sprite width and move speed

typedef struct Entity {
    unsigned short x;
    unsigned short y;
    unsigned char startTileX;
    unsigned char startTileY;
    unsigned char targetTileX;
    unsigned char targetTileY;
    unsigned char currentTileX;
    unsigned char currentTileY;
    unsigned char hasTarget;
    unsigned char visible;
    unsigned char health;
    unsigned char spriteId;
    struct Entity *next;
    struct Entity *prev;
} Entity;

typedef struct Guy {
    unsigned short x;
    unsigned short y;
    unsigned char currentTileX;
    unsigned char currentTileY;
} Guy;

void createMapStatus();
void drawMap();

#endif
