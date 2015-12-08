// Listen for Medtronic packets and print them over the UART

#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "debug.h"
#include "led.h"
#include "radio.h"
#include "serial.h"

void main(void)
{
	clock_init();
	led_init();
	radio_init();
	timer_init();
	use_serial_stdio();
	enable_interrupts();

	printf("Starting receive test\n\n");

	for (;;)
		recv_packet(0);
}
