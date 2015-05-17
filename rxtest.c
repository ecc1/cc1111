// Poll for Medtronic packets and print them over the UART

#include <stdio.h>
#include <string.h>
#include "4b6b.h"
#include "cc1111.h"
#include "clock.h"
#include "delay.h"
#include "led.h"
#include "radio.h"
#include "serial.h"

void print_bytes(const uint8_t *buf, size_t len)
{
	int i;

	for (i = 0; i < len; ++i) {
		printf("%02X ", buf[i]);
		if ((i + 1) % 20 == 0)
			printf("\n");
	}
	if (i % 20 != 0)
		printf("\n");
}

void main(void)
{
	static char __xdata packet[256], data[256];

	clock_init();
	led_init();
	radio_init();
	serial_init();

	while (1) {
		int length, n;

		length = radio_receive(packet, sizeof(packet));
		printf("\nReceived %d-byte packet:\n", length);
		print_bytes(packet, length);

		n = decode_4b6b_length(length);
		memset(data, 0, n);
		if (decode_4b6b(packet, data, length) == 0)
			printf("4b/6b decoded packet:\n");
		else
			printf("FAILED 4b/6b decoding:\n");
		print_bytes(data, n);
	}
}
