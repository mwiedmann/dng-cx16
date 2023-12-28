#ifndef TILES_H
#define TILES_H

#define TILE_TRANS 0
#define TILE_BLACK 1
#define TILE_FLOOR 2
#define TILE_WALL 3
#define TILE_ENTITY 4
#define TILE_GUY 5  
#define TILE_GENERATOR 6
#define TILE_DOOR 7
#define TILE_KEY 8

#define ENTITY_TILE_START 100
#define ENTITY_TILE_END 228
#define ENTITY_CLAIM 254
#define GUY_CLAIM 255

void initTiles();
void clearLayers();
void clearLayer0();
void clearLayer1();
void drawOverlay();
void clearTile(unsigned char x, unsigned char y);

#endif