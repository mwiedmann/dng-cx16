#ifndef INTRO_H
#define INTRO_H

// How many ticks before changing to next instructions screen
#define SCREEN_CHANGE_COUNT 720

typedef struct MessageList {
    unsigned char x;
    unsigned char y;
} MessageList;

void instructions();
void showLevelIntro();

#endif
