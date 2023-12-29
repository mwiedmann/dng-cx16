#ifndef LIST_H
#define LIST_H

#include "globals.h"

void printList(char *name, Entity *list);
void addNewEntityToList(Entity *entity, Entity **to);
void moveEntityToList(Entity *entity, Entity **to, Entity **from);
void deleteEntityFromList(Entity *entity, Entity **list);
Entity *getEntityById(unsigned char spriteId, Entity *list);

#endif