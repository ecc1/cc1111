#ifndef _DEBUG_H
#define _DEBUG_H

#include <stddef.h>
#include <stdint.h>

void recv_packet(void);

void print_bytes(const uint8_t *buf, size_t len);

#endif /* _DEBUG_H */
