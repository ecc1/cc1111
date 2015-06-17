#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "led.h"

void main(void)
{
	uint16_t start;
	const uint16_t interval = 1 * HZ / 2;

	clock_init();
	led_init();
	timer_init();
	enable_interrupts();

	start = time();
	for (;;) {
		uint16_t now = time();
		if (now >= start + interval) {
			led_toggle();
			start = now;
		}
	}
}
