#ifndef SOUND_H
#define SOUND_H

#define SOUND_PRIORITY_MUSIC 0
#define SOUND_PRIORITY_PL1 1
#define SOUND_PRIORITY_PL2 2
#define SOUND_PRIORITY_COMMON 3

#define SOUND_INDEX_NONE 0
#define SOUND_INDEX_TITLE 1
#define SOUND_INDEX_WELCOME 2
#define SOUND_INDEX_KEYS 3
#define SOUND_INDEX_TREASURE 4
#define SOUND_INDEX_SCROLLS 5
#define SOUND_INDEX_FOOD 6
#define SOUND_INDEX_NIGH 7
#define SOUND_INDEX_LAUGH 8

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
void soundPlayMusic(unsigned char index);
void soundPlayVoice(unsigned char index);
void toggleMusic();

extern unsigned char currentMusic;

#endif
