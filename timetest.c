#include <stdio.h>
#include "clock.h"
#include "led.h"

void main(void)
{
	uint16_t start;
	const uint16_t interval = 1 * HZ / 2;

	clock_init();
	led_init();
	timer_init();
	EA = 1;

	start = time();
	while (1) {
		uint16_t now = time();
		if (now >= start + interval) {
			led_toggle();
			start = now;
		}
	}
}
