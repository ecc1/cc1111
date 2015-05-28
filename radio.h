#ifndef _RADIO_H
#define _RADIO_H

#include <stddef.h>
#include <stdint.h>

#include "cc1111.h"

void radio_init(void);

size_t radio_receive(uint8_t *buf, size_t len, uint16_t timeout);

// WARNING: radio_transmit writes a null byte to buf[len]
// so buf must be at least len+1 bytes

void radio_transmit(uint8_t *buf, size_t len);

void radio_txrx_isr(void) __interrupt RFTXRX_VECTOR;

void radio_rf_isr(void) __interrupt RF_VECTOR;

#endif /* _RADIO_H */
