#ifndef UTILS_H
#define UTILS_H

void loadFileToVRAM(char *filename, unsigned long addr);
void loadFileToBankedRAM(char *filename, unsigned char bank, unsigned short addr);
void clearBank(unsigned char bank, unsigned short startingByte);

#endif