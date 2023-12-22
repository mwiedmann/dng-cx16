#ifndef TILES_H
#define TILES_H

#define TILE_EMPTY 0
#define TILE_SOLID 1
#define TILE_ENTITY 2
#define TILE_GUY 3

#define ENTITY_TILE_START 100
#define ENTITY_TILE_END 228
#define ENTITY_CLAIM 254
#define GUY_CLAIM 255

void initTiles();
void clearLayers();
void clearLayer0();
void clearLayer1();

#endif