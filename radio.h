#ifndef _RADIO_H_
#define _RADIO_H_

#include <stddef.h>
#include <stdint.h>

// Configure radio for Medtronic RF

void radio_init(void);

size_t radio_receive(uint8_t *buf, size_t len);

void radio_transmit(const uint8_t *buf, size_t len);

#endif  // _RADIO_H_
