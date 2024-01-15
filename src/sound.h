#ifndef SOUND_H
#define SOUND_H

#define SOUND_PRIORITY_MUSIC 0
#define SOUND_PRIORITY_ATTACK 1
#define SOUND_PRIORITY_ACTION 2

#define SOUND_MUSIC_NONE 0
#define SOUND_MUSIC_TITLE 1

#define SOUND_SFX_BARBATK 0

void soundInit();
void soundPlaySFX(unsigned char effect, unsigned char priority) ;
void soundStopChannel(unsigned char priority);

void soundLoadMusic(unsigned char music);
void soundPlayMusic(unsigned char music);

#endif
