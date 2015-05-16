#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "4b6b.h"

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

#define BUF_SIZE	256

uint8_t input[BUF_SIZE];
uint8_t output[BUF_SIZE];

int main(int argc, char **argv)
{
	int n, len;

	if (argc <= 1 || argc >= BUF_SIZE) {
		fprintf(stderr, "Usage: %s bytes in hex ...\n", argv[0]);
		return 1;
	}
	--argc;
	++argv;

	for (n = 0; n < argc; ++n) {
		int v;
		char *endp;

		v = strtol(*argv, &endp, 16);
		if (*endp != '\0' || v < 0 || v > 0xFF) {
			fprintf(stderr, "Argument '%s' is not a hex byte\n", *argv);
			return 1;
		}
		input[n] = v;
		++argv;
	}

	len = decode_4b6b_length(n);
	memset(output, 0, len);
	if (decode_4b6b(input, output, n))
		printf("Decoding FAILED\n");
	print_bytes(output, len);

	return 0;
}
