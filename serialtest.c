// Print "Hello, world!" over UART

#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "serial.h"

void main(void)
{
	int counter = 0;

	clock_init();
	serial_init();
	enable_interrupts();

	for (;;)
		printf("Hello, world from CC1111! (%u)\n", ++counter);
}
