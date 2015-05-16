#include <stdint.h>
#include <stdio.h>

// Generate lookup table for CRC-8 calculation

void gen_crc8(uint8_t poly)
{
    int i;

    printf("// Lookup table for CRC-8 calculation with polyomial 0x%02X\n\n", poly);
    printf("static uint8_t crc8_lookup[256] = {\n");

    for (i = 0; i < 256; ++i) {
        uint8_t res = i;
        int n;
        for (n = 0; n < 8; ++n) {
            uint8_t c = res & (1 << 7);
            res <<= 1;
            if (c)
                res ^= poly;
        }
        printf("  0x%02X,", res);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }

    printf("};\n");
}

// Generate lookup table for CRC-16 calculation

void gen_crc16(uint16_t poly)
{
    int i;

    printf("// Lookup table for CRC-16 calculation with polynomial 0x%04X\n\n", poly);
    printf("static uint16_t crc16_lookup[256] = {\n");

    for (i = 0; i < 256; ++i) {
        uint16_t res = 0;
        uint16_t b = i << 8;
        int n;
        for (n = 0; n < 8; ++n) {
            uint16_t c = (res ^ b) & (1 << 15);
            res <<= 1;
            b <<= 1;
            if (c)
                res ^= poly;
        }
        printf("  0x%04X,", res);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }

    printf("};\n");
}

int main(void)
{
    gen_crc8(0x9B);	// WCDMA polynomial
    printf("\n");
    gen_crc16(0x1021);	// CCITT polynomial

    return 0;
}
