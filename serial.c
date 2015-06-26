#include "serial.h"

#include "arch.h"
#include "set_stdio.h"

// Based on AltOS cc1111/ao_serial.c

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

void serial_tx_isr(void) __interrupt UTX0_VECTOR
{
	if (!fifo_empty(tx_fifo)) {
		UTX0IF = 0;
		fifo_remove(tx_fifo, U0DBUF);
	} else {
		IEN2 &= ~IEN2_UTX0IE;	// disable UART0 TX interrupt
	}
}

void serial_putc(char c) __critical
{
	while (fifo_full(tx_fifo))
		await_interrupt();
	fifo_insert(tx_fifo, c);
	fifo_remove(tx_fifo, U0DBUF);
	IEN2 |= IEN2_UTX0IE;	// enable UART0 TX interrupt
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

	URX0IE = 1;		// enable UART0 RX interrupt
	IEN2 &= ~IEN2_UTX0IE;	// disable UART0 TX interrupt, but ...
	UTX0IF = 1;		// set TX flag so it will fire when enabled
}

void use_serial_stdio(void)
{
	serial_init();
	set_stdio(serial_getc, serial_putc);
}
