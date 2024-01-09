#include <cx16.h>
#include <joystick.h>

#include "wait.h"

void waitForRelease() {
    unsigned char joy;

    while (1) {
        joy = joy_read(0);
        if (joy == 0) {
            break;
        }
        wait();
    }
}

unsigned char waitForButtonPress() {
    unsigned char joy, pressed;

    while(1) {
        joy = joy_read(0);

        if (JOY_BTN_1(joy) || JOY_BTN_2(joy) || JOY_BTN_3(joy) || JOY_START(joy) || JOY_SELECT(joy)) {
            pressed = joy;

            while(JOY_BTN_1(joy) || JOY_BTN_2(joy) || JOY_BTN_3(joy) || JOY_START(joy) || JOY_SELECT(joy)) {
                wait();
                joy = joy_read(0);
            }
            return pressed;
        }
    }
}