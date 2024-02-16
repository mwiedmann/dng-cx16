#include <cx16.h>
#include <joystick.h>

#include "wait.h"
#include "globals.h"

unsigned char getPlayerInput(unsigned char playerId) {
    unsigned char joy;

    // 0 = KB=player 1, Joy1=player 2
    // 1 = KB/Joy1=player 1, Joy2=player 2
    // 2 = Test Mode = KB/Joy1=control BOTH
    if (controllerMode == 0) {
        joy = joy_read(playerId);
    } else if (controllerMode == 1) {
        joy = playerId == 0
            ? joy_read(0) | joy_read(1)
            : joy_read(2);
    } else {
        joy = joy_read(0) | joy_read(1);
    }

    return joy;
}

#pragma code-name (push, "BANKRAM02")

void waitForRelease() {
    unsigned char joy;

    while (1) {
        joy = joy_read(0) | joy_read(1) | joy_read(2);
        if (joy == 0) {
            break;
        }
        wait();
    }
}

unsigned char waitForButtonPress() {
    unsigned char joy, pressed;

    while(1) {
        joy = joy_read(0) | joy_read(1) | joy_read(2);

        if (JOY_BTN_1(joy) || JOY_BTN_2(joy) || JOY_BTN_3(joy) || JOY_START(joy) || JOY_SELECT(joy)) {
            pressed = joy;

            while(JOY_BTN_1(joy) || JOY_BTN_2(joy) || JOY_BTN_3(joy) || JOY_START(joy) || JOY_SELECT(joy)) {
                wait();
                joy = joy_read(0) | joy_read(1) | joy_read(2);
            }
            return pressed;
        }
    }
}

#pragma code-name (pop)