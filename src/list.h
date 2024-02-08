#ifndef LIST_H
#define LIST_H

#include "globals.h"

void addNewEntityToList(Entity *entity, Entity **to);
void moveEntityToList(Entity *entity, Entity **to, Entity **from);
void deleteEntityFromList(Entity *entity, Entity **list);
Entity *getEntityById(unsigned char tileId, Entity *list);

#endif