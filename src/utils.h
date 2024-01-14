#ifndef UTILS_H
#define UTILS_H

// A pointer to Memory location 0
// which holds the number of the current RAM Bank
#define BANK_NUM (*(unsigned char *)0x00)

void loadFileToVRAM(char *filename, unsigned long addr);
void loadFileToBankedRAM(char *filename, unsigned char bank, unsigned short addr);

#endif