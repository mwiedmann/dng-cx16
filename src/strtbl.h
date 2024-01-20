#ifndef STRTBL_H
#define STRTBL_H

#define STRING_INSTR0_START 0
#define INSTR0_LENGTH 8

#define STRING_INSTR1_START STRING_INSTR0_START + INSTR0_LENGTH
#define INSTR1_LENGTH 9

#define STRING_INSTR2_START STRING_INSTR1_START + INSTR1_LENGTH
#define INSTR2_LENGTH 6

#define STRING_STAT_SHEET_START STRING_INSTR2_START + INSTR2_LENGTH
#define STAT_SHEET_LENGTH 9

#define STRING_INSTR_SELECT_START STRING_STAT_SHEET_START + STAT_SHEET_LENGTH
#define INSTR_SELECT_LENGTH 8

#define STRING_INSTR_CONTROLS_START STRING_INSTR_SELECT_START + INSTR_SELECT_LENGTH
#define INSTR_CONTROLS_LENGTH 15

#define STRING_COUNT INSTR0_LENGTH + INSTR1_LENGTH + INSTR2_LENGTH + STAT_SHEET_LENGTH + INSTR_SELECT_LENGTH + INSTR_CONTROLS_LENGTH

char * getString(unsigned char i);
void loadStrings();

#endif