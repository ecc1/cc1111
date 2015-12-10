#include "debug.h"

#include <stdio.h>
#include <string.h>
#include "4b6b.h"
#include "clock.h"
#include "crc.h"
#include "radio.h"

#ifndef VERBOSE
#define VERBOSE	0
#endif

void print_bytes(const uint8_t *buf, size_t len)
{
	int i;

	for (i = 0; i < len; ++i) {
		printf("%02X ", buf[i]);
#if VERBOSE
		if ((i + 1) % 20 == 0)
			printf("\n");
#endif
	}
#if VERBOSE
	if (i % 20 != 0)
		printf("\n");
#else
	printf("\n");
#endif
}

void print_time(void)
{
	uint16_t t = time();
	uint16_t s = t / HZ;
	uint16_t r = t % HZ;
#if VERBOSE
	putchar('\n');
#endif
	printf("%03d.%02d ", s, r);
}

__xdata static uint8_t packet[256];

uint8_t *recv_packet(int timeout)
{
	__xdata static uint8_t bytes[256];
	int length, n;
	uint8_t crc;

	length = radio_receive(packet, sizeof(packet), timeout);
	if (length == 0) {
#if VERBOSE
		print_time();
		printf("Receive timeout\n");
#endif
		return 0;
	}
#if VERBOSE
	print_time();
	printf("Received %d-byte packet:\n", length);
	print_bytes(packet, length);
#endif
	memset(bytes, 0, sizeof(bytes));
	n = decode_4b6b(packet, bytes, length);
#if VERBOSE
	printf("4b/6b decoding%s:\n", n == -1 ? " FAILED" : "");
#else
	print_time();
	if (n == -1) {
		printf("4b/6b decoding failed\n");
		return 0;
	}
	printf("< ");
#endif
	print_bytes(bytes, n);
	if (n < 2)
		return 0;
	crc = crc8(bytes, n - 1);
	if (bytes[n - 1] != crc)
		printf("CRC should be %02X\n", crc);
	return bytes;
}

void send_packet(uint8_t *buf, size_t len)
{
	int n;

	buf[len - 1] = crc8(buf, len - 1);
	print_time();
#if VERBOSE
	printf("Transmitting packet:\n");
#else
	printf("> ");
#endif
	print_bytes(buf, len);
	n = encode_4b6b(buf, packet, len);
#if VERBOSE
	printf("4b/6b encoding:\n");
	print_bytes(packet, n);
#endif
	radio_transmit(packet, n);
}
