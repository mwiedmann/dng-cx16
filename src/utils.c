#include <cx16.h>
#include <cbm.h>

#include "utils.h"
#include "config.h"

#pragma code-name (push, "BANKRAM02")

void loadFileToVRAM(char *filename, unsigned long addr) {
    // These 3 functions are basic wrappers for the Kernal Functions

    // You have to first set the name of the file you are working with.
    cbm_k_setnam(filename);

    // Next you setup the LFS (Logical File) for the file
    // First param is the Logical File Number
    //   Use 0 if you are just loading the file
    //   You can use other values to keep multiple files open
    // Second param is the device number
    //   The SD Card on the CX16 is 8
    // The last param is the Secondary Address
    // 0 - File has the 2 byte header, but skip it
    // 1 - File has the 2 byte header, use it
    // 2 - File does NOT have the 2 byte header
    cbm_k_setlfs(0, 8, 2);

    // Finally, load the file somewhere into RAM or VRAM
    // First param of cbm_k_load means:
    //   0, loads into system memory.
    //   1, perform a verify.
    //   2, loads into VRAM, starting from 0x00000 + the specified starting address.
    //   3, loads into VRAM, starting from 0x10000 + the specified starting address.
    // Second param is the 16 bit address 
    cbm_k_load(addr > 0xFFFFL ? 3 : 2, addr);
}

#pragma code-name (pop)

void loadFileToBankedRAM(char *filename, unsigned char bank, unsigned short addr) {
    // Set the RAM Bank we are loading into
    BANK_NUM = bank;

    // See the chapter on Files for info on these cbm file functions
    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 2);

    // Reminder, first param of cbm_k_load of "0" means load into system memory.
    // WE WANT THIS ONE because Banked RAM is system memory (at address 0xA000)

    // BANK_RAM is a #define provided by cx16.h
    // as a pointer to 0xA000 (which is where Bank RAM starts)
    cbm_k_load(0, (unsigned short)BANK_RAM + addr);
}

void clearBank(unsigned char bank) {
    unsigned short i;
    unsigned char *ptr;

    BANK_NUM = bank;

    for (i=0; i<8192; i++) {
        ptr = (BANK_RAM + i);
        *ptr = 0;
    }
}