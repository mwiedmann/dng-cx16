#ifndef GLOBALS_H
#define GLOBALS_H

#include "map.h"

// Should be 0 except for testing
#define STARTING_LEVEL 0  
#define PLAYER_STARTING_GOLD 0

#define ENTITY_COUNT 75

// Don't spawn more entities if there are already this many on screen
#define ENTITY_COUNT_LIMIT 25

#define GUY_SPEED_1 1
#define GUY_SPEED_2 2
#define WEAPON_SPEED 4
#define WEAPON_ROTATION_SPEED 4

// Gold/Score values
#define TREASURE_CHEST_GOLD 100
#define GOLD_PILE_GOLD 25
#define SILVER_PILE_GOLD 10
#define TREASURE_CHEST_SCORE 1000
#define GOLD_PILE_SCORE 250
#define SILVER_PILE_SCORE 100

// Shop Prices
#define BOOST_PRICE 1000
#define BOOST_SCORE 5000

#define KEY_PRICE 250
#define SCROLL_PRICE 500
#define BIG_FOOD_PRICE 500
#define SMALL_FOOD_PRICE 250

#define KEY_SCORE 100
#define SCROLL_SCORE 100
#define BIG_FOOD_SCORE 100
#define SMALL_FOOD_SCORE 50


#define NUM_PLAYERS 2

#define SCROLL_TILE_SIZE 15
#define SCROLL_PIXEL_SIZE SCROLL_TILE_SIZE * 16
#define TWO_PLAYER_SCROLL_LIMIT SCROLL_TILE_SIZE * 15

#define MAP_MAX 32
#define MAP_PIXEL_MAX MAP_MAX*16
#define MAP_SCROLL_MAX MAP_PIXEL_MAX - SCROLL_PIXEL_SIZE
#define GUY_MAX MAP_PIXEL_MAX-(16+GUY_SPEED_2) // Right edge - Sprite width and move speed

#define WEAK_HEALTH 4
#define NORMAL_HEALTH 8
#define STRONG_HEALTH 12

#define INVENTORY_LIMIT 10

#define BOOST_ID_SPEED 0
#define BOOST_ID_MELEE 1
#define BOOST_ID_RANGED 2
#define BOOST_ID_MAGIC 3
#define BOOST_ID_ARMOR 4

#define PLAYER_HIT_ANIM_FRAMES 12
#define PLAYER_HIT_ANIM_FRAMES_STOP 8

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
    unsigned char startingHealth;
    unsigned char lob;
    unsigned char rangedRate;
} EntityStats;

// 47 bytes
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
    unsigned char rangedTicks;
    unsigned char isShot;
    unsigned char isLob;
    signed short xDir;
    signed short yDir;
    signed short xLobTarget;
    signed short yLobTarget;
} Entity;

typedef struct PlayerStats {
    unsigned char speeds[6];
    unsigned char meleeDamage;
    unsigned char ticksToMelee;
    unsigned char rangedDamage;
    unsigned char ticksToRanged;
    unsigned char scrollDamage;
    unsigned short startingHealth;
    unsigned char foodHealthBig;
    unsigned char foodHealthSmall;
    unsigned char armor[4];
} PlayerStats;

typedef struct Guy {
    unsigned char active;
    enum Character characterType;
    PlayerStats *stats;
    PlayerStats *boostedStats;
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
    unsigned char animationTile;
    unsigned char weaponTile;
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
    unsigned char *canBoost;
    unsigned char hasBoosts[5]; // speed, melee, ranged, magic, armor
    unsigned char wasHit;
    unsigned char weaponSound;
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

typedef struct Hints {
    unsigned char keys;
    unsigned char treasure;
    unsigned char scrolls;
    unsigned char food;
    unsigned char boosts[5];
} Hints;

extern unsigned char (*mapStatus)[MAP_MAX];

extern Entity *entityList;
extern Entity *entitySleepList;
extern Entity *entityActiveList;
extern Entity *entityTempActiveList;

extern unsigned char activeEntityCount;
extern unsigned char totalEntityCount;

extern Guy players[NUM_PLAYERS];
extern Weapon weapons[NUM_PLAYERS];

extern unsigned short maxMapX;
extern unsigned short maxMapY;

extern EntityStats *entityStatsByType[10];
extern PlayerStats *playerStatsByType[4];
extern PlayerStats *playerBoostedStatsByType[4];

extern unsigned char playerCanBoostByType[4][5];
extern unsigned char playerMoveChunks[4];

extern Hints hints;

extern unsigned char weaponRotation[4];
extern short scrollX;
extern short scrollY;

extern unsigned char overlayChanged;
extern unsigned char activePlayers;

extern unsigned char level;
extern unsigned char isShopLevel;

#endif