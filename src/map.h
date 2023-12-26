#ifndef MAP_H
#define MAP_H

#define SCROLL_TILE_SIZE 15
#define SCROLL_PIXEL_SIZE SCROLL_TILE_SIZE * 16
#define MAP_MAX 90
#define MAP_PIXEL_MAX MAP_MAX*16
#define MAP_SCROLL_MAX MAP_PIXEL_MAX - SCROLL_PIXEL_SIZE
#define GUY_MAX MAP_PIXEL_MAX-(16+GUY_SPEED_2) // Right edge - Sprite width and move speed

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
    unsigned char animationCount;
    unsigned char animationFrame;
} Entity;

typedef struct Guy {
    unsigned short x;
    unsigned short y;
    unsigned char currentTileX;
    unsigned char currentTileY;
    unsigned short health;
    unsigned short score;
} Guy;

void createMapStatus();
void drawMap();

#endif
