#include <cc1110.h>
#include "delay.h"

// Blink LEDs on Ciseco SRF-Stick
//     heartbeat: P1_7
//     TX: P1_6
//     RX: P1_5

#if BOARD == BOARD_SRF_STICK

void main(void)
{
	int state;

	P1DIR |= (1 << 7) | (1 << 6) | (1 << 5);	// enable output

	P1_7 = 0;
	P1_6 = 0;
	P1_5 = 0;

	state = 0;
	for (;;) {
		switch (state) {
		case 0:
			P1_7 ^= 1;
			break;
		case 1:
			P1_6 ^= 1;
			break;
		case 2:
			P1_5 ^= 1;
			break;
		}
		state = (state + 1) % 3;
		delay(250);
	}
}

#elif BOARD == BOARD_TI_DONGLE

void main(void)
{
	P1DIR |= (1 << 1);
	P1_1 = 0;

	for (;;) {
		P1_1 ^= 1;
		delay(250);
	}
}

#endif
