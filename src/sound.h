#ifndef SOUND_H
#define SOUND_H

#define SOUND_PRIORITY_MUSIC 0
#define SOUND_PRIORITY_PL1 0
#define SOUND_PRIORITY_PL2 1

#define SOUND_MUSIC_NONE 0
#define SOUND_MUSIC_TITLE 1
#define SOUND_MUSIC_WELCOME 2
#define SOUND_MUSIC_KEYS 3
#define SOUND_MUSIC_TREASURE 4

#define SOUND_SFX_WEAPON_SWOOSH 0
#define SOUND_SFX_WEAPON_BLAST 1
#define SOUND_SFX_SCROLL 2
#define SOUND_SFX_DAMAGE 3

void soundInit();
void soundPlaySFX(unsigned char effect, unsigned char priority) ;
void soundStopChannel(unsigned char priority);
void soundLoadMusic(unsigned char music);
void soundPlayMusic(unsigned char music);

#endif
