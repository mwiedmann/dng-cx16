#include <cx16.h>
#include <cbm.h>
#include <string.h>

#include "config.h"
#include "strtbl.h"

char * sbuf[40];

char * getString(unsigned char i) {
    unsigned char prevBank = RAM_BANK;
    unsigned short * locationPtr = (unsigned short *)BANK_RAM;
    
    RAM_BANK = STRING_BANK;

    // Copy the string from Banked RAM into a lo-RAM buffer
    // so it can be returned and we can safely switch banks
    strcpy((char *)sbuf, (char *)locationPtr[i]);

    RAM_BANK = prevBank;

    return (char *)sbuf;
}

void loadStrings() {
    RAM_BANK = STRING_BANK;

    cbm_k_setnam("strings.bin");
    cbm_k_setlfs(0, 8, 0); // has 2 byte header because cbm_k_save will add it

    // // Reminder, first param of cbm_k_load of "0" means load into system memory.
    cbm_k_load(0, (unsigned short)BANK_RAM);
}
