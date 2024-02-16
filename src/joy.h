#ifndef JOY_H
#define JOY_H

unsigned char getPlayerInput(unsigned char playerId);
void waitForRelease();
unsigned char waitForButtonPress();

#endif