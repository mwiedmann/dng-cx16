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

#define TILE_EXIT_1 10
#define TILE_SCROLL 11
#define TILE_FOOD_BIG 12
#define TILE_FOOD_SMALL 15
#define TILE_EXIT_5 16
#define TILE_EXIT_10 17

#define TILE_BOOST_START 18
#define TILE_BOOST_END 22

#define TILE_SPEED_BOOST 18
#define TILE_MELEE_BOOST 19
#define TILE_RANGED_BOOST 20
#define TILE_MAGIC_BOOST 21
#define TILE_ARMOR_BOOST 22

#define TILE_GENERATOR_START 23
#define TILE_GENERATOR_END 32
#define TILE_ENTITY_START 33
#define TILE_ENTITY_END 45

#define L1_TILE_TRANS 43
#define L1_TILE_BLACK 44
#define L1_TILE_WHITE 45

#define GUY_TILE_START 56
#define MONSTER_TILE 96
#define MONSTER_PROJECTILE_TILE 136
#define GENERATOR_TILE 47

#define WARLOCKS_DUNGEON_TILE 64
#define CHARACTER_NAME_TILES 144

#define ENTITY_TILE_START 100
#define ENTITY_TILE_END 228
#define ENTITY_CLAIM 253
#define GUY_CLAIM 254

void loadDungeonTiles();
unsigned char letterToTile(char letter);
void clearLayer0();
void clearLayer0VisibleArea();
void clearLayer1VisibleArea();
void clearVisibleLayers();
void updateCharacterTypeInOverlay(unsigned char playerId);
void drawOverlayBackground();
void updateOverlay();
void copyTile(unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY);
void flashLayer1();
void l0TileShow(unsigned char x, unsigned char y, unsigned char tile);
unsigned char shopQuestion(unsigned short cost, unsigned char stringId);
void gameMessage(unsigned char stringId, unsigned char sound);
void message(unsigned char x, unsigned char y, char *msg);
void messageCenter(char *msg[4]);
// unsigned char gameQuestion(char *msg1, char *msg2);

#endif