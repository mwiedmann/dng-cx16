#include <stdio.h>
#include "list.h"
#include "globals.h"
#include "sprites.h"

// This file has a linked list implementation AND some entity functions that use it
// I combined them because they all live in the same Banked RAM file

// These functions are put at the beginning of BANK 1 along with mapStatus/entityList
// Be sure to monitor the size of this code so it doesn't smash into those data structures
// I've allowed 2k of room
#pragma code-name (push, "BANKRAM01")

void addNewEntityToList(Entity *entity, Entity **to) {
    // Add to the beginning of the new list
    // There is never a prev since 1st Entity in list
    entity->prev = 0;

    // If empty list, point to NULL
    // Else point to the 1st Entity in the list
    if (!(*to)) {    
        entity->next = 0;
    } else {
        entity->next = *to;
        (*to)->prev = entity;
    }

    // Enity becomes the new head of the list
    *to = entity;
}

void moveEntityToList(Entity *entity, Entity **to, Entity **from) {
    // If the entity is first in the list, point the list to the next entity (if any, could be 0)
    if (*from == entity) {
        *from = entity->next;
    }

    // Connect the entities around this entity together
    if (entity->prev) {
        entity->prev->next = entity->next;
    }
    if (entity->next) {
        entity->next->prev = entity->prev;
    }

    // Add to the beginning of the new list
    // There is never a prev since 1st Entity in list
    entity->prev = 0;

    // If empty list, point to NULL
    // Else point to the 1st Entity in the list
    if (!(*to)) {    
        entity->next = 0;
    } else {
        entity->next = *to;
        (*to)->prev = entity;
    }

    // Enity becomes the new head of the list
    *to = entity;
}

void deleteEntityFromList(Entity *entity, Entity **list) {
    // If the entity is first in the list, point the list to the next entity (if any, could be 0)
    if (*list == entity) {
        *list = entity->next;
    }

    // Connect the entities around this entity together
    if (entity->prev) {
        entity->prev->next = entity->next;
    }
    if (entity->next) {
        entity->next->prev = entity->prev;
    }
}

Entity *getEntityById(unsigned char spriteId, Entity *list) {
    Entity *entity;

    entity = list;

    while(entity) {
        if (entity->spriteId == spriteId) {
            return entity;
        }
        entity = entity->next;
    }

    return 0;
}

void activateEntities() {
    Entity *entity;
    Entity *nextEntity;

    // Start the active list at 0
    activeEntityCount = 0;
    totalEntityCount = 0;

    entity = entitySleepList;
    entityTempActiveList = 0;
    
    // Make sure we have at least 1 sleeping entity or there is nothing to do
    if (!entity) {
        return;
    }

    do {
        totalEntityCount += 1;
        nextEntity = entity->next;

        // Activate this entity if it is on the screen
        if (entity->x >= scrollX + SCROLL_PIXEL_SIZE || entity->x+16 <= scrollX || entity->y >= scrollY + SCROLL_PIXEL_SIZE || entity->y+16 <= scrollY) {
            // Off screen
        } else {
            entity->visible = 1;
            moveAndSetAnimationFrame(entity->spriteAddrLo, entity->spriteAddrHi, entity->spriteGraphicLo, entity->spriteGraphicHi,
                entity->x, entity->y, entity->animationFrame, entity->facingX, 0);
            moveEntityToList(entity, &entityTempActiveList, &entitySleepList);
        }

        entity = nextEntity;
    } while (entity);
}

void deactivateEntities() {
    Entity *entity;
    Entity *nextEntity;

    entity = entityActiveList;
    
    // Make sure we have at least 1 active entity or there is nothing to do
    if (!entity) {
        // printf("No active entities to deactivate\n");
        return;
    }

    do {
        totalEntityCount+= 1;
        nextEntity = entity->next;

        // Deactivate this entity if it is out of range
        if (entity->x >= scrollX + SCROLL_PIXEL_SIZE || entity->x+16 <= scrollX || entity->y >= scrollY + SCROLL_PIXEL_SIZE || entity->y+16 <= scrollY) {
            // Off screen
            toggleSprite(entity->spriteAddrLo, entity->spriteAddrHi, 0);
            entity->visible = 0;
            moveEntityToList(entity, &entitySleepList, &entityActiveList);
        } else {
            activeEntityCount += 1;
        }

        entity = nextEntity;
    } while (entity);
}

void tempActiveToActiveEntities() {
    Entity *entity;
    Entity *nextEntity;

    entity = entityTempActiveList;
    
    // Now for the final step, move the entityTempActiveList into the activeList

    // Make sure we have at least 1 active entity or there is nothing to do
    if (!entity) {
        // printf("No tempActive entities to copy to active\n");
        return;
    }

    do {
        nextEntity = entity->next;
        
        moveEntityToList(entity, &entityActiveList, &entityTempActiveList);

        activeEntityCount += 1;

        entity = nextEntity;
    } while (entity);

    entityTempActiveList = 0;
}

#pragma code-name (pop)