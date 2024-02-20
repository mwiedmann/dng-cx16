/*
 * SPDX-FileCopyrightText: 2023 Toboter
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "sound.h"
#include "config.h"

#include <cbm.h>
#include <cx16.h>

unsigned char sfxAddressHigh[] = {0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4};

unsigned char currentMusic = SOUND_MUSIC_NONE;
unsigned char loadedMusic = SOUND_MUSIC_NONE;

unsigned char musicOn = 1;
unsigned char sfxOn = 1;

unsigned char param1, param2;

char * musicNames[] = {
	"",
	"title.zsm",
	"welcome.zsm",
	"keys.zsm",
	"treasure.zsm",
	"scrolls.zsm",
	"food.zsm",
	"nigh.zsm",
	"laugh.zsm",
	"door.zsm"
};

unsigned char musicBanks[] = {
	MUSIC_BANK_START,
	MUSIC_BANK_TITLE,
	MUSIC_BANK_WELCOME,
	MUSIC_BANK_KEYS,
	MUSIC_BANK_TREASURE,
	MUSIC_BANK_SCROLLS,
	MUSIC_BANK_FOOD,
	MUSIC_BANK_NIGH,
	MUSIC_BANK_LAUGH
};

void loadSound(char* name, unsigned char index) {
	cbm_k_setlfs(0, 8, 2);
	cbm_k_setnam(name);
	cbm_k_load(0, ((unsigned short)sfxAddressHigh[index])<<8);
}

void soundLoadMusic(unsigned char music) {
	unsigned char bank;
	unsigned char prevBank = RAM_BANK;
	param2 = SOUND_PRIORITY_MUSIC;

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");

	bank = musicBanks[music];

	RAM_BANK = bank;

	cbm_k_setlfs(0, 8, 2);
	cbm_k_setnam(musicNames[music]);
	cbm_k_load(0, 0xa000);
	loadedMusic = music;
	RAM_BANK = prevBank;
}

void soundInit() {
	unsigned char i=0;
	asm volatile ("lda #%b", ZSM_BANK);
	asm volatile ("jsr zsm_init_engine");
	asm volatile ("jsr zsmkit_setisr");

	RAM_BANK = SFX_BANK_1;

	loadSound("wpnswp1.zsm", SOUND_SFX_WEAPON_SWOOSH_P1);
	loadSound("wpnswp2.zsm", SOUND_SFX_WEAPON_SWOOSH_P2);
	loadSound("wpnblp1.zsm", SOUND_SFX_WEAPON_BLAST_P1);
	loadSound("wpnblp2.zsm", SOUND_SFX_WEAPON_BLAST_P2);
	loadSound("scroll.zsm", SOUND_SFX_SCROLL);
	loadSound("keyget.zsm", SOUND_SFX_KEY_GET);
	loadSound("eating.zsm", SOUND_SFX_EATING);
	loadSound("hit.zsm", SOUND_SFX_DAMAGE);
	loadSound("demon.zsm", SOUND_SFX_DEMON);
	loadSound("teleport.zsm", SOUND_SFX_TELEPORT);
	loadSound("door.zsm", SOUND_SFX_DOOR);

	for (i=1; i<=9; i++) {
		soundLoadMusic(i);
	}
}

void soundPlaySFX(unsigned char effect, unsigned char priority) {
	unsigned char prevBank = RAM_BANK;

	if (!sfxOn) {
		return;
	}
	
	RAM_BANK = SFX_BANK_1;

	param1 = sfxAddressHigh[effect];
	param2 = priority;

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");

	asm volatile ("lda #$00");
	asm volatile ("ldx %v", param2);
	asm volatile ("ldy %v", param1); //address hi to Y
	asm volatile ("jsr zsm_setmem");

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_play");

	RAM_BANK = prevBank;
}

void soundStopChannel(unsigned char priority) {
	currentMusic = 0;
	param2 = priority;
	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");
}

void soundPlayVoice(unsigned char music) {
	unsigned char bank;
	unsigned char prevBank = RAM_BANK;

	bank = musicBanks[music];

	param1 = 0xa0;
	param2 = SOUND_PRIORITY_COMMON;

	RAM_BANK = bank;

	asm volatile ("lda #$00");
	asm volatile ("ldx %v", param2);
	asm volatile ("ldy %v", param1); //address hi to Y
	asm volatile ("jsr zsm_setmem");


	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_play");

	RAM_BANK = prevBank;
}

void soundPlayMusic(unsigned char music) {
	unsigned char bank;
	unsigned char prevBank = RAM_BANK;

	if (!musicOn || music == currentMusic) {
		return;
	}

	bank = musicBanks[music];

	currentMusic = music;

	param1 = 0xa0;
	param2 = SOUND_PRIORITY_MUSIC;

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");

	if (!music) {
		return;
	}

	loadedMusic = music;
	RAM_BANK = bank;

	asm volatile ("lda #$00");
	asm volatile ("ldx %v", param2);
	asm volatile ("ldy %v", param1); //address hi to Y
	asm volatile ("jsr zsm_setmem");


	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_play");

	asm volatile ("ldx %v", param2); //music loops
	asm volatile ("sec");
	asm volatile ("jsr zsm_setloop");

	RAM_BANK = prevBank;
}

void toggleMusic() {
	soundStopChannel(SOUND_PRIORITY_MUSIC);

	musicOn = !musicOn;
}
