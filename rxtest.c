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

	enable_interrupts();

	while (1)
		recv_packet();
}
