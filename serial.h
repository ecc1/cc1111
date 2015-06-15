#ifndef _SERIAL_H
#define _SERIAL_H

#include "cc1111.h"

void serial_init(void);

char serial_getc(void);

void serial_putc(char);

void serial_rx_isr(void) __interrupt URX0_VECTOR;

void serial_tx_isr(void) __interrupt UTX0_VECTOR;

#endif /* _SERIAL_H */
