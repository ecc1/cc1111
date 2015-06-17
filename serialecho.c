// Echo characters over UART

#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "serial.h"

void main(void)
{
	clock_init();
	serial_init();
	enable_interrupts();

	for (;;)
		putchar(getchar());
}
