#ifndef TILES_H
#define TILES_H

#define TILE_TRANS 0
#define TILE_BLACK 1
#define TILE_FLOOR 2
#define TILE_WALL 3
#define TILE_GUY 5 
#define TILE_DOOR 7
#define TILE_KEY 8
#define TILE_TREASURE_CHEST 9
#define TILE_TREASURE_GOLD 13
#define TILE_TREASURE_SILVER 14

#define TILE_EXIT 10
#define TILE_SCROLL 11
#define TILE_FOOD_BIG 12
#define TILE_FOOD_SMALL 15

#define TILE_GENERATOR_START 20
#define TILE_GENERATOR_END 32
#define TILE_ENTITY_START 33
#define TILE_ENTITY_END 45

#define L1_TILE_TRANS 46
#define L1_TILE_BLACK 47
#define L1_TILE_WHITE 45

#define AXE_TILE 69
#define GUY_TILE_START 64
#define MONSTER_TILE 128
#define GENERATOR_TILE 56

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
void updateCharacterTypeInOverlay(unsigned char playerId);
void drawOverlayBackground();
void updateOverlay();
void copyTile(unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY);
void flashLayer1();
void l0TileShow(unsigned char x, unsigned char y, unsigned char tile);
void gameMessage(unsigned char x1, unsigned char y1, char *msg1, unsigned char x2, unsigned char y2, char *msg2);
void message(unsigned char x, unsigned char y, char *msg);

#endif