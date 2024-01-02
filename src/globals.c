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

unsigned char entitySlowMove[4] = { 2, 1, 1, 1 };
unsigned char entityMediumMove[4] = { 2, 1, 2, 1 };
unsigned char entityFastMove[4] = { 2, 2, 2, 2 };


EntityStats skeletonStats = {
    entityWeakMelee,
    entityMediumMove,
    0, // ranged
    NORMAL_HEATH, // health
};

EntityStats slimeStats = {
    entityWeakMelee,
    entityMediumMove,
    0, // ranged
    WEAK_HEALTH, // health
};

EntityStats snakeStats = {
    entityNormalMelee,
    entityMediumMove,
    0, // ranged
    NORMAL_HEATH, // health
};

EntityStats dragonStats = {
    entityNormalMelee,
    entityMediumMove,
    15, // ranged
    NORMAL_HEATH, // health
};

EntityStats spiderStats = {
    entityStrongMelee,
    entityMediumMove,
    0, // ranged
    NORMAL_HEATH, // health
};

EntityStats golemStats = {
    entityStrongMelee,
    entitySlowMove,
    0, // ranged
    STRONG_HEALTH, // health
};

EntityStats batStats = {
    entityWeakMelee,
    entityFastMove,
    0, // ranged
    WEAK_HEALTH, // health
};

EntityStats beholderStats = {
    entityWeakMelee,
    entityFastMove,
    15, // ranged
    NORMAL_HEATH, // health
};

EntityStats demonStats = {
    entityStrongMelee,
    entityMediumMove,
    0, // ranged
    STRONG_HEALTH, // health
};

EntityStats insectsStats = {
    entityWeakMelee,
    entityFastMove,
    0, // ranged
    WEAK_HEALTH, // health
};

EntityStats *entityStatsByType[10] = {
    &skeletonStats,
    &slimeStats,
    &snakeStats,
    &dragonStats,
    &spiderStats,
    &golemStats,
    &batStats,
    &beholderStats,
    &demonStats,
    &insectsStats
};


/*
    unsigned char speeds[4];
    unsigned char meleeDamage;
    unsigned char ticksToMelee;
    unsigned char rangedDamage;
    unsigned char ticksToRanged;
    unsigned char scrollDamage;
    unsigned char startingHealth;
    unsigned char foodHealth;
    unsigned char armor[4];
*/

/*
        BARBARIAN    MAGE    DRUID    RANGER
Speed      *         **      ***      ****
Melee      ****      *       **       ***
Ranged
 Damage    ****      ***     **       **
 Rate      *         ****    **       ****
Magic      *         ****    ****     **
Health     ****      *       ***      **
Armor      **        *       ****     **
*/

PlayerStats barbarianStats = { {1, 1, 1, 2}, 1, 5  /* 12-sec */,  8, 15 /* 32-sec */,  4, 1000, 200, { 0, 0, 0, 1 } };
PlayerStats mageStats =      { {1, 1, 1, 2}, 1, 10 /* 6-sec */,   6, 10 /* 36-sec */, 12,  700, 100, { 0, 0, 0, 0 } };
PlayerStats druidStats =     { {2, 1, 2, 1}, 1, 7  /* 8.5-sec */, 4, 12 /* 20-sec */, 12,  900, 150, { 0, 0, 1, 1 } };
PlayerStats rangerStats =    { {2, 2, 2, 1}, 1, 6  /* 10-sec */,  4, 10 /* 24-sec */,  8,  800, 125, { 0, 0, 0, 1 } };

// Barb, Mage, Druid, Ranger
PlayerStats *playerStatsByType[4] = {
    &barbarianStats,
    &mageStats,
    &druidStats,
    &rangerStats
};

Hints hints = { 0, 0, 0 };

unsigned char weaponRotation[4] = {0, 1, 3, 2};
short scrollX, scrollY;
unsigned char overlayChanged = 0;