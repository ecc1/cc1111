// Print "Hello, world!" over USB

#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "usb.h"

void main(void)
{
	int counter = 0;

	clock_init();
	use_usb_stdio();
	enable_interrupts();

	for (;;) {
		printf("Hello, world from CC1111! (%u)\n", ++counter);
		usb_flush();
		run_background_tasks();
	}
}
