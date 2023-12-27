#include <stdio.h>
#include "list.h"
#include "globals.h"

void printList(char *name, Entity *list) {
    Entity *entity;

    entity = list;
    
    printf("%s: ", name);

    while (entity) {
        printf("%i ", entity->spriteId);
        entity = entity->next;
    }

    printf("\n");
}

void addNewEntityToList(Entity *entity, Entity **to) {
    // Add to the beginning of the new list
    // There is never a prev since 1st Entity in list
    entity->prev = 0;

    // If empty list, point to NULL
    // Else point to the 1st Entity in the list
    if (!(*to)) {    
        // printf("List is empty, new HEAD: %i\n", entity->spriteId);
        entity->next = 0;
    } else {
        // printf("List has entities, new HEAD: %i points to: %i\n", entity->spriteId, (*to)->spriteId);
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
        // printf("List is empty, new HEAD: %i\n", entity->spriteId);
        entity->next = 0;
    } else {
        // printf("List has entities, new HEAD: %i points to: %i\n", entity->spriteId, (*to)->spriteId);
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