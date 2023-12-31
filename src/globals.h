#ifndef GLOBALS_H
#define GLOBALS_H

#include "map.h"

#define ENTITY_COUNT 75
#define GUY_SPEED_1 1
#define GUY_SPEED_2 2
#define WEAPON_SPEED 4
#define WEAPON_ROTATION_SPEED 4
#define GUY_SHOOT_TICKS 10
#define WEAPON_DAMAGE 4
#define MELEE_DAMAGE 1

#define NUM_PLAYERS 2

#define SCROLL_TILE_SIZE 15
#define SCROLL_PIXEL_SIZE SCROLL_TILE_SIZE * 16
#define MAP_MAX 32
#define MAP_PIXEL_MAX MAP_MAX*16
#define MAP_SCROLL_MAX MAP_PIXEL_MAX - SCROLL_PIXEL_SIZE
#define GUY_MAX MAP_PIXEL_MAX-(16+GUY_SPEED_2) // Right edge - Sprite width and move speed

#define WEAK_HEALTH 4
#define NORMAL_HEATH 8
#define STRONG_HEALTH 12

enum Character {
  BARBARIAN,
  MAGE,
  DRUID,
  RANGER
};

typedef struct EntityStats {
    unsigned char *melee;
    unsigned char *moves;
    unsigned char ranged;
    unsigned startingHealth;
} EntityStats;

typedef struct Entity {
    EntityStats *stats;
    unsigned char statsId;
    unsigned short isGenerator;
    unsigned short entityTypeId;
    unsigned short tileId;
    unsigned char spawnRate;
    unsigned char nextSpawn;
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
    unsigned char animationChange;
    unsigned char movedPrevTick;
    unsigned char facingX;
    unsigned short points;
} Entity;

typedef struct Guy {
    unsigned char active;
    enum Character characterType;
    unsigned short x;
    unsigned short y;
    unsigned char currentTileX;
    unsigned char currentTileY;
    unsigned short health;
    unsigned short score;
    unsigned short gold;
    unsigned char animationCount;
    unsigned char animationFrame;
    unsigned char animationChange;
    unsigned char facingX;
    signed char aimX;
    signed char aimY;
    signed char pressedX;
    signed char pressedY;
    unsigned char pressedShoot;
    unsigned char pressedScroll;
    unsigned char shooting;
    unsigned char ticksUntilNextShot;
    unsigned char ticksUntilNextMelee;
    unsigned char keys;
    unsigned char scrolls;
    unsigned char exit;
} Guy;

typedef struct Weapon {
    unsigned short x;
    unsigned short y;
    unsigned char visible;
    short dirX;
    short dirY;
    unsigned char animationCount;
    unsigned char animationFrame;
} Weapon;

extern unsigned char (*mapStatus)[MAP_MAX];

extern Entity entityList[ENTITY_COUNT];
extern Entity *entitySleepList;
extern Entity *entityActiveList;
extern Entity *entityTempActiveList;

extern Guy players[NUM_PLAYERS];
extern Weapon weapons[NUM_PLAYERS];

extern unsigned short maxMapX;
extern unsigned short maxMapY;

extern EntityStats *entityStatsByType[10];

#endif