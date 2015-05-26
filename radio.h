#ifndef _RADIO_H
#define _RADIO_H

#include <stddef.h>
#include <stdint.h>

#include "cc1111.h"

void radio_init(void);

size_t radio_receive(uint8_t *buf, size_t len, uint16_t timeout);

void radio_transmit(uint8_t *buf, size_t len);

void radio_txrx_isr(void) __interrupt RFTXRX_VECTOR;

#endif /* _RADIO_H */
