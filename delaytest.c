// Toggle pin 1 of the test connector on the TI CC1111 USB dongle,
// for timing measurement.

#include <cc1110.h>
#include "clock.h"
#include "delay.h"

void main(void)
{
    clock_init();

    P2DIR |= (1 << 0);  // enable output on P2_0
    while (1) {
        P2_0 ^= 1;  // toggle it
        delay(10);
    }
}
