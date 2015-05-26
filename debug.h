#ifndef _DEBUG_H
#define _DEBUG_H

#include <stddef.h>
#include <stdint.h>

uint8_t *recv_packet(int timeout);

void send_packet(uint8_t *buf, size_t len);

void print_bytes(const uint8_t *buf, size_t len);

#endif /* _DEBUG_H */
