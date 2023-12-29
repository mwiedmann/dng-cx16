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
#define TILE_CHEST 9

#define L1_TILE_TRANS 46
#define L1_TILE_BLACK 47

#define TILE_CHARS_START 128

#define WARLOCKS_DUNGEON_TILE 96
#define BARBARIAN_NAME_TILE 160

#define ENTITY_TILE_START 100
#define ENTITY_TILE_END 228
#define ENTITY_CLAIM 254
#define GUY_CLAIM 255

// 4 types: BARBARIAN, MAGE, PALADIN, RANGER

void initTiles();
unsigned char letterToTile(char letter);
void clearLayers();
void clearLayer0();
void clearLayer1();
void drawOverlayBackground();
void updateOverlay();
void copyTile(unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY);
#endif