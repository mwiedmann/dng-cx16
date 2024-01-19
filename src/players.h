#ifndef PLAYERS_H
#define PLAYERS_H

void setGuyDirection(unsigned char playerId);
void moveGuy(unsigned char playerId, unsigned char speed);
void setupPlayer(unsigned char playerId, enum Character characterType);
void moveWeapon(unsigned char playerId);
unsigned char destroyPlayer(unsigned char playerId);
void stopMove(unsigned char playerId);

#endif