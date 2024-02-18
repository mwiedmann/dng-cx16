#ifndef SOUND_H
#define SOUND_H

#define SOUND_PRIORITY_MUSIC 2
#define SOUND_PRIORITY_PL1 0
#define SOUND_PRIORITY_PL2 1
#define SOUND_PRIORITY_COMMON 3

#define SOUND_MUSIC_NONE 0
#define SOUND_MUSIC_TITLE 1
#define SOUND_MUSIC_WELCOME 2
#define SOUND_MUSIC_KEYS 3
#define SOUND_MUSIC_TREASURE 4
#define SOUND_MUSIC_SCROLLS 5
#define SOUND_MUSIC_FOOD 6
#define SOUND_MUSIC_NIGH 7
#define SOUND_MUSIC_LAUGH 8

#define SOUND_SFX_WEAPON_SWOOSH_P1 0
#define SOUND_SFX_WEAPON_SWOOSH_P2 1
#define SOUND_SFX_WEAPON_BLAST_P1 2
#define SOUND_SFX_WEAPON_BLAST_P2 3
#define SOUND_SFX_SCROLL 4
#define SOUND_SFX_KEY_GET 5
#define SOUND_SFX_EATING 6
#define SOUND_SFX_DAMAGE 7
#define SOUND_SFX_DEMON 8
#define SOUND_SFX_TELEPORT 9
#define SOUND_SFX_DOOR 10

#define NIGH_HEALTH 100

void soundInit();
void soundPlaySFX(unsigned char effect, unsigned char priority) ;
void soundStopChannel(unsigned char priority);
void soundLoadMusic(unsigned char music);
void soundPlayMusic(unsigned char music);

extern unsigned char currentMusic;

#endif
