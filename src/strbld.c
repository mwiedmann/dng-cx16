#include <cx16.h>
#include <cbm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "strtbl.h"

char *stringTable[STRING_COUNT] = {
    // Instructions 0
    "WELCOME TO WARLOCK'S DUNGEON",
    "MEET YOUR HEROES",
    "FIGHT YOUR WAY THROUGH",
    "ENDLESS HORDES OF MONSTERS",
    "FIND THE EXIT ON EACH LEVEL",
    "CAN YOU SURVIVE AND ESCAPE?",
    "USE THE JOYPAD TO READ MORE",
    "PRESS A BUTTON TO BEGIN",

    // Instructions 1
    "KNOW YOUR DUNGEONS",
    "USE KEYS TO OPEN DOORS",
    "SPEND TREASURE ON UPGRADES",
    "SPEED   MELEE   RANGED",
    "MAGIC   ARMOR",
    "UPGRADES EXPIRE UPON",
    "REACHING THE NEXT SHOP",
    "SAVE GOLD TO BUY MORE!",
    "USE THE JOYPAD TO READ MORE",
    "PRESS A BUTTON TO BEGIN",

    // Instructions 2
    "KNOW YOUR HEROES",
    "YOUR LIFE SLOWLY TICKS AWAY",
    "EAT FOOD TO REPLENISH",
    "YOUR HEALTH",
    "USE THE JOYPAD TO READ MORE",
    "PRESS A BUTTON TO BEGIN",

    // Instructions 3
    "KNOW YOUR MAGIC", // 15
    "BEWARE OF THE",
    "INDESTRUCTABLE DEVIL",
    "USE MAGIC SCROLLS TO",
    "DESTROY DEVILS AND",
    "DAMAGE ALL VISIBLE ENEMIES",
    "TELEPORTERS TRANSPORT YOU",
    "TO OTHER TELEPORTERS",
    "USE THE JOYPAD TO READ MORE",
    "PRESS A BUTTON TO BEGIN",

    // Stat Sheet
    "      BARB. MAGE DRUID RANGER",
    "SPEED  *    **   **    ****",
    "MELEE  **** *    **    ***",
    "RANGED",
    "   DMG **** ***  *      *",
    "  RATE *    **   ***   ****",
    "MAGIC  *    **** ****  **",
    "HEALTH **** *    ***   **",
    "ARMOR  **   *    ****  **",

    // Selection screen
    "PLAYERS: PRESS A DIRECTION",
    "TO ACTIVATE YOUR HERO",
    "BARBARIAN",
    "DRUID",
    "RANGER",
    "MAGE",
    "WHEN ALL PLAYERS ARE READY",
    "PRESS START-RETURN",

    // Controls screen
    "CONTROLS",
    "MOVEMENT",
    "JOYPAD OR CURSOR KEYS",
    "MELEE ATTACK",
    "MOVE INTO MONSTER",
    "RANGED ATTACK",
    "KEYBOARD: X OR CTRL",
    "  JOYPAD: A OR BUTTON 1",
    "USE MAGIC SCROLL",
    "KEYBOARD: Z OR ALT",
    "  JOYPAD: B OR BUTTON 2",
    "PAUSE",
    "KEYBOARD: SHIFT",
    "  JOYPAD: SELECT",
    "PRESS A BUTTON TO BEGIN",

    // Boosts
    "IMPROVED SPEED",
    "MUSHROOM INCREASES",
    "PLAYER SPEED",
    "IMPROVED MELEE ATTACKS",
    "WEAPON IMPROVES",
    "PLAYER MELEE ATTACKS",
    "IMPROVED RANGED ATTACKS",
    "WEAPON IMPROVES",
    "PLAYER RANGED ATTACKS",
    "IMPROVED SCROLL POWER",
    "RING IMPROVES",
    "PLAYER SCROLL POWER",
    "IMPROVED ARMOR",
    "MITHRIL IMPROVES",
    "PLAYER ARMOR",

    // Game and Shop Messages
    "YOU CANNOT AFFORD",
    "THIS ITEM",
    "COLLECT KEYS",
    "TO OPEN DOORS",
    "KEY",
    "COLLECT TREASURE TO",
    "SPEND ON UPGRADES",
    "USE SCROLLS TO DAMAGE",
    "ALL VISIBLE ENEMIES",
    "SCROLL",
    "YOUR CHARACTER IS ALREADY",
    "GIFTED IN THIS AREA",
    "EAT FOOD TO",
    "GAIN HEALTH",
    "LARGE MEAL",
    "SMALL MEAL",
};

void createStringTable() {
    unsigned char i;
    unsigned short stringLocation[STRING_COUNT];
    unsigned short totalStringLength = 0;
    char * stringPtr = (char *)(BANK_RAM + sizeof(stringLocation));

    RAM_BANK = 1;

    // Create an array of the string lengths and copy the strings
    for (i=0; i<STRING_COUNT; i++) {
        stringLocation[i] = (unsigned short)stringPtr;
        strcpy(stringPtr, stringTable[i]); // +1 for the null terminator

        stringPtr+= strlen(stringTable[i]) + 1;
        totalStringLength + strlen(stringTable[i]) + 1;
    }

    // Put the string length array at the beginning of Banked RAM
    memcpy(BANK_RAM, stringLocation, sizeof(stringLocation));

    cbm_k_setnam("strings.bin");
    // // SAVE adds the 2 byte header and we can't stop it
    cbm_k_setlfs(0, 8, 0);
    cbm_k_save((unsigned short)BANK_RAM, (unsigned short)stringPtr);
}



void main() {
    unsigned char i;

    createStringTable();
    printf("STRINGS.BIN created\n\n");

    loadStrings();
    // Print out the loaded strings from Banked RAM as a test confirmation
    printf("Testing strings from STRINGS.BIN\n\n");
    for (i=0; i<STRING_COUNT; i++) {
        printf("%s\n", getString(i));
    }
}