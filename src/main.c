#include <stdio.h>
#include "sound.h"

void main() {
    unsigned long i;

    soundInit();

    soundPlayMusic(SOUND_MUSIC_TITLE);

    printf("Voice will play in a moment.\nThe music will get trashed.\n");
    for (i=0; i<300000; i++) {}

    soundPlayVoice(SOUND_MUSIC_WELCOME);
}