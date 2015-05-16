#include <stdio.h>
#include <stdlib.h>
#include "crc.h"

int main(int argc, char **argv)
{
    uint8_t buf[256];
    int n;

    if (argc <= 1 || argc >= sizeof(buf)) {
        fprintf(stderr, "Usage: %s bytes in hex ...\n", argv[0]);
        return 1;
    }

    for (n = 0; n < argc - 1; ++n) {
        int v;
        char *endp;

        v = strtol(argv[n + 1], &endp, 16);
        if (*endp != '\0' || v < 0 || v > 0xFF) {
            fprintf(stderr, "Argument '%s' is not a hex byte\n", argv[n + 1]);
            return 1;
        }
        buf[n] = v;
    }

    printf("CRC-8 = %02X\n", crc8(buf, n));
    printf("CRC-16 = %04X\n", crc16(buf, n));

    return 0;
}
