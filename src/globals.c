#include <cx16.h>

#include "map.h"
#include "globals.h"

// Point this array to hi-ram
unsigned char (*mapStatus)[MAP_MAX] = (unsigned char (*)[MAP_MAX])BANK_RAM;

Entity entityList[ENTITY_COUNT];

Entity *entitySleepList = 0;
Entity *entityActiveList = 0;
Entity *entityTempActiveList = 0;

Guy players[NUM_PLAYERS];
Weapon weapons[NUM_PLAYERS];

unsigned short maxMapX;
unsigned short maxMapY;

unsigned char entityWeakMelee[4] = { 0, 1, 1, 1 };
unsigned char entityNormalMelee[4] = { 1, 1, 1, 1 };
unsigned char entityStrongMelee[4] = { 2, 1, 1, 1 };

unsigned char entityNoMove[4] = { 0, 0, 0, 0 };
unsigned char entitySlowMove[4] = { 2, 1, 1, 1 };
unsigned char entityMediumMove[4] = { 2, 1, 2, 1 };
unsigned char entityFastMove[4] = { 2, 2, 2, 2 };

// Melee Only Monsters
EntityStats skeletonStats = {
    entityWeakMelee,
    entitySlowMove,
    0, // ranged
    NORMAL_HEALTH, // health
    0, 0 // lob, rate
};

EntityStats batStats = {
    entityWeakMelee,
    entityFastMove,
    0,
    WEAK_HEALTH,
    0, 0
};

EntityStats snakeStats = {
    entityNormalMelee,
    entityMediumMove,
    0,
    NORMAL_HEALTH,
    0, 0
};

EntityStats spiderStats = {
    entityStrongMelee,
    entityMediumMove,
    0,
    NORMAL_HEALTH,
    0, 0
};

EntityStats golemStats = {
    entityStrongMelee,
    entitySlowMove,
    0,
    STRONG_HEALTH,
    0, 0
};

EntityStats demonStats = {
    entityStrongMelee,
    entityMediumMove,
    0,
    STRONG_HEALTH,
    0, 0
};

// Ranged attackers
EntityStats insectsStats = {
    entityWeakMelee,
    entityFastMove,
    5,
    WEAK_HEALTH,
    0, 30
};

EntityStats dragonStats = {
    entityNormalMelee,
    entityMediumMove,
    10,
    NORMAL_HEALTH,
    0, 30
};

EntityStats beholderStats = {
    entityNormalMelee,
    entityFastMove,
    15,
    STRONG_HEALTH,
    0, 30
};

// Lobbers
EntityStats slimeStats = {
    entityWeakMelee,
    entityMediumMove,
    10,
    WEAK_HEALTH,
    1, 15
};

EntityStats *entityStatsByType[10] = {
    &skeletonStats,
    &dragonStats,
    &snakeStats,
    &golemStats,
    &spiderStats,
    &beholderStats,
    &batStats,
    &insectsStats,
    &demonStats,
    &slimeStats
};


/*
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
*/

/*
        BARBARIAN    MAGE    DRUID    RANGER
Speed      *         **      **       ****
Melee      ****      *       **       ***
Ranged
 Damage    ****      ***     **       **
 Rate      *         ****    **       ****
Magic      *         ****    ****     **
Health     ****      *       ***      **
Armor      **        *       ****     **
*/

PlayerStats barbarianStats = { {1, 1, 1, 1, 1, 1}, 1, 5  /* 12-sec */,  8, 25 /* 19-sec */,  4, 1000, 200, 75, { 0, 0, 0, 1 } };
PlayerStats mageStats =      { {1, 1, 1, 1, 1, 2}, 1, 10 /* 6-sec */,   6, 20 /* 18-sec */, 12,  700, 100, 10, { 0, 0, 0, 0 } };
PlayerStats druidStats =     { {1, 1, 1, 1, 1, 2}, 1, 7  /* 8.5-sec */, 4, 15 /* 16-sec */, 12,  900, 150, 50, { 0, 0, 1, 1 } };
PlayerStats rangerStats =    { {1, 1, 1, 1, 2, 2}, 1, 6  /* 10-sec */,  4, 12 /* 20-sec */,  8,  800, 125, 25, { 0, 0, 0, 1 } };

PlayerStats boostedBarbarianStats = { {1, 1, 1, 1, 1, 2}, 1, 5  /* 12-sec */,  8, 20 /* 24-sec */,  8, 1000, 200, 75, { 0, 0, 1, 1 } };
PlayerStats boostedMageStats =      { {1, 1, 1, 1, 2, 2}, 1, 7 /* 8.5-sec */,   8, 15 /* 32-sec */, 12,  700, 100, 10, { 0, 0, 0, 1 } };
PlayerStats boostedDruidStats =     { {1, 1, 1, 1, 2, 2}, 1, 6  /* 10-sec */, 6, 10 /* 36-sec */, 12,  900, 150, 50, { 0, 0, 1, 1 } };
PlayerStats boostedRangerStats =    { {1, 1, 1, 1, 2, 2}, 1, 5  /* 12-sec */,  6, 8 /* 45-sec */,  12,  800, 125, 25, { 0, 0, 1, 1 } };

unsigned char playerMoveChunks[4] = { 6, 7, 7, 8 };

// Barb, Mage, Druid, Ranger
PlayerStats *playerStatsByType[4] = {
    &barbarianStats,
    &mageStats,
    &druidStats,
    &rangerStats
};

// Barb, Mage, Druid, Ranger
PlayerStats *playerBoostedStatsByType[4] = {
    &boostedBarbarianStats,
    &boostedMageStats,
    &boostedDruidStats,
    &boostedRangerStats
};

// What boosts can a character type use.
// e.g. Barbarian cannot boost his melee...he is already maxed
unsigned char playerCanBoostByType[4][5] = {
    {1, 0, 1, 1, 1},
    {1, 1, 1, 0, 1},
    {1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1}
};

Hints hints = { 0, 0, 0, 0 };

unsigned char weaponRotation[4] = {0, 1, 3, 2};
signed short scrollX, scrollY;
unsigned char overlayChanged = 0;
unsigned char activePlayers=0;
unsigned char level=0;
unsigned char isShopLevel = 0;
