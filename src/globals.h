#ifndef GLOBALS_H
#define GLOBALS_H

#include "map.h"

// Uncomment this to skip a few things during testing (title screen, welcome sound, etc)
// #define TEST_MODE

#define STARTING_LEVEL 1 // Should be 1 except for testing
#define PLAYER_STARTING_GOLD 0 // Should be 0 except for testing

// Once the game passes this level, go back to 1
#define LEVEL_COUNT 50

#define ENTITY_COUNT 75

// Don't spawn more entities if there are already this many on screen
#define ENTITY_SCREEN_COUNT_LIMIT_1P 30
#define ENTITY_SCREEN_COUNT_LIMIT_2P 20 // 2 players eats up more cycles, need to limit entities

// All doors will open after some time
#define OPEN_ALL_DOORS_TICKS 9000 // 2.5 mins * 60 ticks per second

#define WEAPON_SPEED 4
#define WEAPON_ROTATION_SPEED 4

// Gold/Score values
#define TREASURE_CHEST_GOLD 150
#define GOLD_PILE_GOLD 75
#define SILVER_PILE_GOLD 25

#define TREASURE_CHEST_SCORE 1500
#define GOLD_PILE_SCORE 750
#define SILVER_PILE_SCORE 250

// Shop Prices
#define BOOST_PRICE 1000
#define BOOST_SCORE 5000

#define KEY_PRICE 250
#define SCROLL_PRICE 500
#define BIG_FOOD_PRICE 500
#define SMALL_FOOD_PRICE 250

#define KEY_SCORE 250
#define SCROLL_SCORE 1000
#define BIG_FOOD_SCORE 1000
#define SMALL_FOOD_SCORE 500

#define NUM_PLAYERS 2

#define SCROLL_TILE_SIZE 15
#define SCROLL_PIXEL_SIZE SCROLL_TILE_SIZE * 16
#define TWO_PLAYER_SCROLL_LIMIT SCROLL_TILE_SIZE * 15

#define MAP_MAX 32
#define MAP_PIXEL_MAX MAP_MAX*16
#define MAP_SCROLL_MAX MAP_PIXEL_MAX - SCROLL_PIXEL_SIZE

#define WEAK_HEALTH 4
#define NORMAL_HEALTH 8
#define STRONG_HEALTH 12
#define INDESTRUCTABLE_HEALTH 255

#define INVENTORY_LIMIT 10

#define BOOST_ID_SPEED 0
#define BOOST_ID_MELEE 1
#define BOOST_ID_RANGED 2
#define BOOST_ID_MAGIC 3
#define BOOST_ID_ARMOR 4

#define PLAYER_HIT_ANIM_FRAMES 12
#define PLAYER_HIT_ANIM_FRAMES_STOP 8

#define ENTITY_HIT_ANIM_FRAMES 4
#define ENTITY_HIT_ANIM_FRAMES_STOP 2
#define ENTITY_HIT_PAL_JUMP 5

// How many entities are moved after the activation/deactivation phase.
// There are a few cycles left so we call moveEntity for a few entities.
// For 2 player games (due to the cycles eaten up by the 2nd player) there are
// fewer cycles left, so we do a smaller amount.
#define ENTITY_SPLIT_AMOUNT_1P 15
#define ENTITY_SPLIT_AMOUNT_2P 5

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
    unsigned char points;
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
    unsigned short targetTilePixelX;
    unsigned short targetTilePixelY;
    unsigned char currentTileX;
    unsigned char currentTileY;
    unsigned char hasTarget;
    unsigned char visible;
    unsigned char health;
    unsigned char spriteId;
    unsigned short spriteAddrLo;
    unsigned char spriteAddrHi;
    unsigned char spriteGraphicLo;
    unsigned char spriteGraphicHi;
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
    unsigned char wasHit;
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
    unsigned long score;
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
    unsigned short spriteAddrLo;
    unsigned char spriteAddrHi;
    unsigned char spriteGraphicLo;
    unsigned char spriteGraphicHi;
    unsigned char teleportTileX;
    unsigned char teleportTileY;
} Guy;

typedef struct Weapon {
    unsigned short x;
    unsigned short y;
    unsigned char visible;
    short dirX;
    short dirY;
    unsigned char animationCount;
    unsigned char animationFrame;
    unsigned short spriteAddrLo;
    unsigned char spriteAddrHi;
    unsigned char spriteGraphicLo;
    unsigned char spriteGraphicHi;
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
extern signed short scrollX;
extern signed short scrollY;
extern signed short compositeScrollXOffset;
extern signed short compositeScrollYOffset;
extern unsigned char hscale;
extern unsigned char vscale;

extern unsigned char overlayChanged;
extern unsigned char activePlayers;

extern unsigned char level;
extern unsigned char isShopLevel;

extern unsigned char demonHitting;
extern unsigned char demonSoundOn;

extern unsigned char controllerMode;
#endif