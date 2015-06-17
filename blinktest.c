#include <cc1110.h>
#include "delay.h"

// Blink LEDs on Ciseco SRF-Stick
//     heartbeat: P1_7
//     tx: P1_6
//     rx: P1_5

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
