#include <stdio.h>
#include <string.h>
#include "arch.h"
#include "clock.h"
#include "crc.h"
#include "debug.h"
#include "delay.h"
#include "dma.h"
#include "led.h"
#include "pump_id.h"
#include "radio.h"
#include "serial.h"

#ifndef PUMP_ID
#error "PUMP_ID is undefined"
#endif

#define PDIGIT(i)	(PUMP_ID[i] - '0')

#define PUMP_ID_1	(PDIGIT(0) << 4 | PDIGIT(1))
#define PUMP_ID_2	(PDIGIT(2) << 4 | PDIGIT(3))
#define PUMP_ID_3	(PDIGIT(4) << 4 | PDIGIT(5))

#define COMMAND_SIZE	7
__xdata uint8_t command[COMMAND_SIZE + 1];	// include padding

#define DEVICE_PUMP	0xA7

#define WAKE_UP		0x5D
#define ACK		0x06
#define GET_BATTERY	0x72
#define GET_PUMP_MODEL	0x8D

void main(void)
{
	const int retries = 200;
	int n;
	uint8_t *p;

	clock_init();
	dma_init();
	led_init();
	radio_init();
	timer_init();
	use_serial_stdio();
	enable_interrupts();

	printf("Starting transmit test\n\n");

	command[0] = DEVICE_PUMP;
	command[1] = PUMP_ID_1;
	command[2] = PUMP_ID_2;
	command[3] = PUMP_ID_3;

	command[4] = WAKE_UP;
	for (n = 0; n < retries; ++n) {
		send_packet(command, COMMAND_SIZE);
		p = recv_packet(3);
		if (p && p[4] == ACK) {
			printf("ACK after %d wakeups\n", n);
			break;
		}
		led_toggle();
	}

	command[4] = GET_PUMP_MODEL;
	for (n = 0; n < retries; ++n) {
		send_packet(command, COMMAND_SIZE);
		p = recv_packet(25);
		if (p && p[4] == GET_PUMP_MODEL) {
			printf("Pump model: %c%c%c (retries: %d)\n", p[7], p[8], p[9], n);
			break;
		}
		led_toggle();
	}

	command[4] = GET_BATTERY;
	for (n = 0; n < retries; ++n) {
		send_packet(command, COMMAND_SIZE);
		p = recv_packet(25);
		if (p && p[4] == GET_BATTERY) {
			printf("Battery: %d.%02dV (retries: %d)\n", p[8] / 100, p[8] % 100, n);
			break;
		}
		led_toggle();
	}

	blink(250, 0);
}
