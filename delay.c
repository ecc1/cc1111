#include "delay.h"

#include "arch.h"

void delay(uint16_t msec)
{
	while (msec--) {
		uint16_t n = 825;	// determined empirically
		while (n--)
			nop();
	}
}
