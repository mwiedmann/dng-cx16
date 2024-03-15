/*
 * SPDX-FileCopyrightText: 2023 Toboter
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// To convert raw sound to a .ZCM
// ./raw2zcm -q 8000 ../sound/welcome.raw ../sound/welcome.zcm

#include "sound.h"
#include "config.h"
#include "utils.h"

#include <cbm.h>
#include <cx16.h>

unsigned char sfxAddressHigh[] = {0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4};

unsigned char currentMusic = SOUND_INDEX_NONE;
unsigned char loadedMusic = SOUND_INDEX_NONE;

unsigned char musicOn = 1;
unsigned char sfxOn = 1;

unsigned char param1, param2;

char * soundFileNames[] = {
	"",
	"title.zsm",
	"game1.zsm",
	"game2.zsm",
	"game3.zsm",
	"game4.zsm",
	"welcome.zcm",
	"keys.zcm",
	"treasure.zcm",
	"scrolls.zcm",
	"food.zcm",
	"nigh.zcm",
	"laugh.zcm"
};

unsigned char soundBanks[] = {
	SOUND_BANK_START,
	SOUND_BANK_MUSIC, // Title
	SOUND_BANK_MUSIC, // Game1,2,3,4
	SOUND_BANK_MUSIC,
	SOUND_BANK_MUSIC,
	SOUND_BANK_MUSIC,
	SOUND_BANK_WELCOME,
	SOUND_BANK_KEYS,
	SOUND_BANK_TREASURE,
	SOUND_BANK_SCROLLS,
	SOUND_BANK_FOOD,
	SOUND_BANK_NIGH,
	SOUND_BANK_LAUGH
};

void volume(unsigned char volume, unsigned char priority) {
	param1 = priority;
	param2 = volume; // 0=FULL - 63=MUTE

	asm volatile ("ldx %v", param1);
	asm volatile ("lda %v", param2);
	asm volatile ("jsr zsm_setatten");
}

void loadZCM(unsigned char index) {
	unsigned char bank;
	unsigned char prevBank = RAM_BANK;

	bank = soundBanks[index];

	RAM_BANK = bank;

	cbm_k_setlfs(0, 8, 2);
	cbm_k_setnam(soundFileNames[index]);
	cbm_k_load(0, 0xa000);

	RAM_BANK = prevBank;
}

void loadSound(char* name, unsigned char index) {
	cbm_k_setlfs(0, 8, 2);
	cbm_k_setnam(name);
	cbm_k_load(0, ((unsigned short)sfxAddressHigh[index])<<8);
}

void soundLoadMusic(unsigned char index) {
	unsigned char bank;
	unsigned char prevBank = RAM_BANK;
	param2 = SOUND_PRIORITY_MUSIC;

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");

	bank = soundBanks[index];

	RAM_BANK = bank;

	cbm_k_setlfs(0, 8, 2);
	cbm_k_setnam(soundFileNames[index]);
	cbm_k_load(0, 0xa000);
	loadedMusic = index;
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

	soundLoadMusic(SOUND_INDEX_TITLE);

	for (i=6; i<=12; i++) {
		loadZCM(i);
	}
}

void soundStopChannel(unsigned char priority) {
	if (priority == SOUND_PRIORITY_MUSIC) {
		currentMusic = 0;
	}
	
	param2 = priority;
	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");
}

void soundPlayVoice(unsigned char index) {
	unsigned char bank;
	unsigned char prevBank = RAM_BANK;

	bank = soundBanks[index];

	param1 = 0xa0;
	param2 = 0; // Slot

	RAM_BANK = bank;

	asm volatile ("lda #$00");
	asm volatile ("ldx %v", param2); // Slot
	asm volatile ("ldy %v", param1); //address hi to Y
	asm volatile ("jsr zcm_setmem");

	asm volatile ("lda #$0f");
	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zcm_play");

	RAM_BANK = prevBank;
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

void soundPlayMusic(unsigned char index) {
	unsigned char bank;
	unsigned char prevBank = RAM_BANK;

	if (!musicOn || index == currentMusic) {
		return;
	}

	asm volatile ("lda #%b", ZSM_BANK);
	asm volatile ("jsr zsm_init_engine");

	soundLoadMusic(index);

	bank = soundBanks[index];

	currentMusic = index;

	param1 = 0xa0;
	param2 = SOUND_PRIORITY_MUSIC;

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");

	if (!index) {
		return;
	}

	loadedMusic = index;
	RAM_BANK = bank;

	asm volatile ("lda #$00");
	asm volatile ("ldx %v", param2);
	asm volatile ("ldy %v", param1); //address hi to Y
	asm volatile ("jsr zsm_setmem");

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_play");

	volume(54, SOUND_PRIORITY_MUSIC);

	asm volatile ("ldx %v", param2); //music loops
	asm volatile ("sec");
	asm volatile ("jsr zsm_setloop");

	RAM_BANK = prevBank;
}

void toggleMusic(unsigned char trackIndex) {
	musicOn = !musicOn;

	if (musicOn) {
		soundPlayMusic(trackIndex);
	} else {
		soundStopChannel(SOUND_PRIORITY_MUSIC);
	}
}
