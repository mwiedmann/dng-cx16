#ifndef STRTBL_H
#define STRTBL_H

#define STRING_INSTR0_START 0
#define INSTR0_LENGTH 8

#define STRING_INSTR1_START STRING_INSTR0_START + INSTR0_LENGTH
#define INSTR1_LENGTH 10

#define STRING_INSTR2_START STRING_INSTR1_START + INSTR1_LENGTH
#define INSTR2_LENGTH 6

#define STRING_INSTR3_START STRING_INSTR2_START + INSTR2_LENGTH
#define INSTR3_LENGTH 10

#define STRING_STAT_SHEET_START STRING_INSTR3_START + INSTR3_LENGTH
#define STAT_SHEET_LENGTH 9

#define STRING_INSTR_CONTROL_SCHEME_START STRING_STAT_SHEET_START + STAT_SHEET_LENGTH
#define INSTR_CONTROL_SCHEME_LENGTH 10

#define STRING_INSTR_SELECT_START STRING_INSTR_CONTROL_SCHEME_START + INSTR_CONTROL_SCHEME_LENGTH
#define INSTR_SELECT_LENGTH 8

#define STRING_INSTR_CONTROLS_START STRING_INSTR_SELECT_START + INSTR_SELECT_LENGTH
#define INSTR_CONTROLS_LENGTH 15

#define STRING_BOOSTS_START STRING_INSTR_CONTROLS_START + INSTR_CONTROLS_LENGTH
#define BOOSTS_LENGTH 15

#define STRING_GAME_MSG_START STRING_BOOSTS_START + BOOSTS_LENGTH
#define GAME_MSG_LENGTH 17

#define STRING_COUNT INSTR0_LENGTH + INSTR1_LENGTH + INSTR2_LENGTH + INSTR3_LENGTH + STAT_SHEET_LENGTH + INSTR_CONTROL_SCHEME_LENGTH + INSTR_SELECT_LENGTH + INSTR_CONTROLS_LENGTH + BOOSTS_LENGTH + GAME_MSG_LENGTH

#define STRING_GAME_MSG_CANNOT_AFFORD STRING_GAME_MSG_START
#define STRING_GAME_MSG_COLLECT_KEYS STRING_GAME_MSG_CANNOT_AFFORD + 2
#define STRING_GAME_MSG_KEY STRING_GAME_MSG_COLLECT_KEYS + 2
#define STRING_GAME_MSG_COLLECT_TREASURE STRING_GAME_MSG_KEY+1
#define STRING_GAME_MSG_USE_SCROLLS STRING_GAME_MSG_COLLECT_TREASURE+2
#define STRING_GAME_MSG_SCROLL STRING_GAME_MSG_USE_SCROLLS+2
#define STRING_GAME_MSG_ALREADY_GIFTED STRING_GAME_MSG_SCROLL+1
#define STRING_GAME_MSG_EAT_FOOD STRING_GAME_MSG_ALREADY_GIFTED+2
#define STRING_GAME_MSG_LARGE_MEAL STRING_GAME_MSG_EAT_FOOD+2
#define STRING_GAME_MSG_SMALL_MEAL STRING_GAME_MSG_LARGE_MEAL+1
#define STRING_GAME_MSG_PAUSED STRING_GAME_MSG_SMALL_MEAL + 1

char * getString(unsigned char i);
char * getLevelString(unsigned char i);
void loadStrings();

#endif