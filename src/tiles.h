#ifndef TILES_H
#define TILES_H

#define TILE_TRANS 0
#define TILE_BLACK 1
#define TILE_FLOOR 2
#define TILE_WALL 3
#define TILE_GUY 5 
#define TILE_DOOR 7
#define TILE_KEY 8
#define TILE_CHEST 9
#define TILE_EXIT 10
#define TILE_GENERATOR_START 20
#define TILE_GENERATOR_END 32
#define TILE_ENTITY_START 33
#define TILE_ENTITY_END 45

#define L1_TILE_TRANS 46
#define L1_TILE_BLACK 47

#define TILE_CHARS_START 128

#define WARLOCKS_DUNGEON_TILE 96
#define CHARACTER_NAME_TILES 176

#define ENTITY_TILE_START 100
#define ENTITY_TILE_END 228
#define ENTITY_CLAIM 253
#define GUY_CLAIM 254

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