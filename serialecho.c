// Echo characters over UART

#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "serial.h"

void main(void)
{
	clock_init();
	use_serial_stdio();
	enable_interrupts();

	for (;;)
		putchar(getchar());
}
