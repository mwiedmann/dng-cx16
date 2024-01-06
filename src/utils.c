#include <stdlib.h>
#include <cx16.h>
#include <cbm.h>

#include "utils.h"
#include "config.h"
#include "globals.h"

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

void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned long vramAddr, unsigned long length) {
    unsigned long remaining;
    unsigned short i;

    VERA.address = vramAddr;
    VERA.address_hi = vramAddr>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // This crazy routine uses the kernal memory_copy function to bulk copy RAM to VRAM
    // I had to increment the bank and do it in chunks though.
    for (i=0; i<length/8192+1; i++) {
        // Set the RAM Bank we are reading from
        BANK_NUM = startMemBank+i;

         // Virtual Registers for the Kernal Function
        R0 = (unsigned short)BANK_RAM; // Read from the Banked RAM starting Address 0xA000
        R1 = 0x9F23; // Write to the VERA.data0 memory location

        remaining = length - (8192*i);
        if (remaining < 8192) {
            R2 = remaining;
        } else {
            R2 = 8192;
        }

        // Call the memory_copy Kernal Function
        __asm__("jsr $FEE7");
    }
}


unsigned char posLessThan(unsigned short posA, unsigned short posB) {
    if (levelWrap) {
        /*
        If B>A, then use method:
        A to right = B-A
        A to left = A+(16-B)

        If B<A, then use method:
        A to left = A-B
        A to right = B+(16-A)
        */
        return posA == posB
            ? 0
            : posB > posA
                ? posB-posA < posA+(MAP_PIXEL_MAX-posB)
                : posB+(MAP_PIXEL_MAX-posA) < posA-posB;
    } else {
        return posA < posB;
    }
}

unsigned char tileLessThan(unsigned char tileA, unsigned char tileB) {
    if (levelWrap) {
        /*
        If B>A, then use method:
        A to right = B-A
        A to left = A+(16-B)

        If B<A, then use method:
        A to left = A-B
        A to right = B+(16-A)
        */
        return tileA == tileB
            ? 0
            : tileB > tileA
                ? tileB-tileA < tileA+(MAP_MAX-tileB)
                : tileB+(MAP_MAX-tileA) < tileA-tileB;
    } else {
        return tileA < tileB;
    }
}

unsigned char posGreaterThan(unsigned char posA, unsigned posB) {
    if (levelWrap) {
        /*
        If B>A, then use method:
        A to right = B-A
        A to left = A+(16-B)

        If B<A, then use method:
        A to left = A-B
        A to right = B+(16-A)
        */
        return posA == posB
            ? 0
            : posB > posA
                ? posB-posA > posA+(MAP_PIXEL_MAX-posB)
                : posB+(MAP_PIXEL_MAX-posA) > posA-posB;
    } else {
        return posA > posB;
    }
}

unsigned char tileGreaterThan(unsigned char tileA, unsigned tileB) {
    if (levelWrap) {
        /*
        If B>A, then use method:
        A to right = B-A
        A to left = A+(16-B)

        If B<A, then use method:
        A to left = A-B
        A to right = B+(16-A)
        */
        return tileA == tileB
            ? 0
            : tileB > tileA
                ? tileB-tileA > tileA+(MAP_MAX-tileB)
                : tileB+(MAP_MAX-tileA) > tileA-tileB;
    } else {
        return tileA > tileB;
    }
}

unsigned char tileDistance(unsigned char tileA, unsigned tileB) {
    unsigned char tempA, tempB;

    if (levelWrap) {
        /*
        If B>A, then use method:
        A to right = B-A
        A to left = A+(16-B)

        If B<A, then use method:
        A to left = A-B
        A to right = B+(16-A)
        */
        if (tileB > tileA) {
            tempA = tileB-tileA;
            tempB = tileA+(MAP_MAX-tileB);
            return tempA > tempB ? tempB : tempA;
        } else {
            tempA = tileB+(MAP_MAX-tileA);
            tempB = tileA-tileB;
            return tempA > tempB ? tempB : tempA;
        }
    } else {
        return abs(tileA - tileB);
    }
}

unsigned char offScreen(unsigned short x, unsigned short y) {
    if (levelWrap) {
        if (scrollX + SCROLL_PIXEL_SIZE >= 512 /* X is wrapping*/) {
            if (x+16 < scrollX && x > (SCROLL_PIXEL_SIZE - (MAP_PIXEL_MAX-scrollX))) {
                return 1;
            }
        } else if (x >= scrollX + SCROLL_PIXEL_SIZE || x+16 <= scrollX) {
            return 1;
        }
        
        if (scrollY + SCROLL_PIXEL_SIZE >= 512 /* Y is wrapping*/) {
            if (y+16 < scrollY && y > (SCROLL_PIXEL_SIZE - (MAP_PIXEL_MAX-scrollY))) {
                return 1;
            }
        } else if (y >= scrollY + SCROLL_PIXEL_SIZE || y+16 <= scrollY) {
            return 1;
        }

        return 0;
     } else {
        // No level wrap, scrolling should be normal
        return (x >= scrollX + SCROLL_PIXEL_SIZE || x+16 <= scrollX || y >= scrollY + SCROLL_PIXEL_SIZE || y+16 <= scrollY);
     }
}
