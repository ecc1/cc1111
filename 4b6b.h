#ifndef _4B6B_H
#define _4B6B_H

#include <stddef.h>
#include <stdint.h>

// Encode bytes using 4b/6b encoding.

void encode_4b6b(const uint8_t *src, uint8_t *dst, size_t len);

// Number of bytes written to destination when encoding len bytes.

size_t encode_4b6b_length(size_t);

// Decode bytes using 4b/6b encoding.
// Return 0 if successful, or -1 if invalid input was encountered.

int decode_4b6b(const uint8_t *src, uint8_t *dst, size_t len);

// Number of bytes written to destination when decoding len bytes.

size_t decode_4b6b_length(size_t);

#endif /* _4B6B_H */
