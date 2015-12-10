// Based on src/cc1111/ao_serial.c from git://git.gag.com/fw/altos
//
// Copyright © 2009 Keith Packard <keithp@keithp.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.

#include "serial.h"

#include "arch.h"
#include "set_stdio.h"

#define FIFO_SIZE	64

typedef struct {
	uint8_t insert;
	uint8_t remove;
	char fifo[FIFO_SIZE];
} fifo_t;

#define fifo_empty(f)	((f).insert == (f).remove)

#define fifo_full(f)	((((f).insert + 1) % FIFO_SIZE) == (f).remove)

#define fifo_insert(f, c)					\
	do {							\
		(f).fifo[(f).insert] = (c);			\
		(f).insert = ((f).insert + 1) % FIFO_SIZE;	\
	} while (0)

#define fifo_remove(f, c)					\
	do {							\
		(c) = (f).fifo[(f).remove];			\
		(f).remove = ((f).remove + 1) % FIFO_SIZE;	\
	} while (0)

__xdata static volatile fifo_t rx_fifo;

void serial_rx_isr(void) __interrupt URX0_VECTOR
{
	if (!fifo_full(rx_fifo))
		fifo_insert(rx_fifo, U0DBUF);
}

int serial_pollc(void) __critical
{
	uint8_t c;

	if (fifo_empty(rx_fifo))
		return -1;
	fifo_remove(rx_fifo, c);
	return c;
}

char serial_getc(void)
{
	int c;

	while ((c = serial_pollc()) == -1)
		idle();
	return c;
}

__xdata static volatile fifo_t tx_fifo;
static volatile uint8_t tx_started;

static void serial_tx_start(void)
{
	if (!tx_started && !fifo_empty(tx_fifo)) {
		tx_started = 1;
		fifo_remove(tx_fifo, U0DBUF);
	}
}

void serial_tx_isr(void) __interrupt UTX0_VECTOR
{
	UTX0IF = 0;
	tx_started = 0;
	serial_tx_start();
}

void serial_putc(char c) __critical
{
	while (fifo_full(tx_fifo))
		await_interrupt();
	fifo_insert(tx_fifo, c);
	serial_tx_start();
}

void serial_init(void)
{
	// Set up UART0 using Alternative Location 1
	PERCFG = (PERCFG & ~PERCFG_U0CFG_ALT_MASK) | PERCFG_U0CFG_ALT_1;

	// UART mode with receiver enabled
	U0CSR |= UxCSR_MODE_UART| UxCSR_RE;

	// Select peripheral function rather than GPIO for TX on P0_3
	// (not required for RX on P0_2)
	P0SEL |= (1 << 3);

	// TX on P0_3, RX on P0_2
	P0DIR |= (1 << 3);
	P0DIR &= ~(1 << 2);

#if BAUD_RATE == 9600

	U0BAUD = 163;
	U0GCR = 8;

#elif BAUD_RATE == 57600

	U0BAUD = 59;
	U0GCR = 11;

#elif BAUD_RATE == 115200

	U0BAUD = 59;
	U0GCR = 12;

#else
#error "unknown BAUD_RATE"
#endif

	U0UCR = (UxUCR_FLUSH |
		 UxUCR_FLOW_DISABLE |
		 UxUCR_D9_EVEN_PARITY |
		 UxUCR_BIT9_8_BITS |
		 UxUCR_PARITY_DISABLE |
		 UxUCR_SPB_1_STOP_BIT | UxUCR_STOP_HIGH | UxUCR_START_LOW);

	IEN2 |= IEN2_UTX0IE;	// enable UART0 TX interrupt
	URX0IE = 1;		// enable UART0 RX interrupt
}

void use_serial_stdio(void)
{
	serial_init();
	set_stdio(serial_getc, serial_putc);
}
