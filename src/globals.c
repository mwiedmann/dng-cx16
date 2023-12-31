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
