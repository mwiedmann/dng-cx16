#ifndef UTILS_H
#define UTILS_H

// A pointer to Memory location 0
// which holds the number of the current RAM Bank
#define BANK_NUM (*(unsigned char *)0x00)

// Pointers to mem 2/4/6 which are "virtual registers"
// for Kernal Functions, used to pass/get data
#define R0 (*(unsigned short *)0x02)
#define R1 (*(unsigned short *)0x04)
#define R2 (*(unsigned short *)0x06)

void loadFileToVRAM(char *filename, unsigned long addr);
void loadFileToBankedRAM(char *filename, unsigned char bank, unsigned short addr);
void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned long vramAddr, unsigned long length);
void restoreRow(char* save, unsigned char row0, unsigned char row1, short scrollY, unsigned short zoomMode);

unsigned char posLessThan(unsigned short posA, unsigned short posB);
unsigned char tileLessThan(unsigned char tileA, unsigned char tileB);
unsigned char posGreaterThan(unsigned char posA, unsigned posB);
unsigned char tileGreaterThan(unsigned char tileA, unsigned tileB);

unsigned char tileDistance(unsigned char tileA, unsigned tileB);
unsigned char offScreen(unsigned short x, unsigned short y);

#endif