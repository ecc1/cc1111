// Listen for Medtronic packets and print them over the UART

#include <stdio.h>
#include <string.h>
#include "arch.h"
#include "4b6b.h"
#include "clock.h"
#include "crc.h"
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
	static uint8_t __xdata packet[256], data[256];

	clock_init();
	led_init();
	radio_init();
	serial_init();

	enable_interrupts();

	while (1) {
		int length, n, err;
		uint8_t crc;

		length = radio_receive(packet, sizeof(packet));
		for (n = 0; n < length; ++n) {
			if (packet[n] == 0x00) {
				length = n;
				break;
			}
		}
		printf("\nReceived %d-byte packet:\n", length);
		print_bytes(packet, length);

		n = decode_4b6b_length(length);
		memset(data, 0, n);
		err = decode_4b6b(packet, data, length);
		printf("4b/6b decoding%s:\n", err ? " FAILED" : "");
		print_bytes(data, n);
		if (err || n < 2)
			continue;
		crc = crc8(data, n - 1);
		if (data[n - 1] != crc)
			printf("CRC failed, should be %02X\n", crc);
	}
}
