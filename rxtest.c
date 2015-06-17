// Listen for Medtronic packets and print them over the UART

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
	serial_init();
	timer_init();

	enable_interrupts();

	for (;;)
		recv_packet(0);
}
