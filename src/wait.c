// This MUST be changed by the VSYNC IRQ handler
unsigned char waitStatus = 0;

void wait() {
    while (!waitStatus);
    waitStatus = 0;
}

void waitCount(unsigned short count) {
    unsigned short i;

    for (i=0; i<count; i++) {
        wait();
    }
}
