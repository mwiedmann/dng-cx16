#ifndef LIST_H
#define LIST_H

#include "map.h"

void printList(char *name, Entity *list);
void moveEntityToList(Entity *entity, Entity **to, Entity **from);

#endif