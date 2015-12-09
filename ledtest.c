#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "led.h"

void main(void)
{
	clock_init();
	led_init();
	enable_interrupts();

	for (;;) {
		int i;
		for (i = 1; i <= 5; i++)
			blink(1000 / i, i);
	}
}
