#ifndef _RADIO_H
#define _RADIO_H

#include <stddef.h>
#include <stdint.h>

#include "cc1111.h"

// Configure radio for Medtronic RF

void radio_init(void);

size_t radio_receive(uint8_t *buf, size_t len);

void radio_transmit(const uint8_t *buf, size_t len);

void radio_txrx_isr(void) __interrupt RFTXRX_VECTOR;

void radio_rf_isr(void) __interrupt RF_VECTOR;

#endif /* _RADIO_H */
