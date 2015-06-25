// Echo characters over USB

#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "usb.h"

void main(void)
{
	clock_init();
	use_usb_stdio();
	enable_interrupts();

	for (;;) {
		putchar(getchar());
		usb_flush();
	}
}
