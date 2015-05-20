#include "radio.h"

#include "arch.h"
#include "rf.h"

void radio_init(void)
{
	// Set up RF registers
	rf_init();

	// Enable radio interrupts
	RFTXRXIE = 1;

	RFIF = 0;
	RFIM = RFIM_IM_DONE;
	IEN2 |= IEN2_RFIE;
}

#define FIFO_SIZE	256

typedef struct {
	uint8_t insert;
	uint8_t remove;
	char fifo[FIFO_SIZE];
} fifo_t;

#define fifo_empty(f)	((f).insert == (f).remove)

#define fifo_full(f)	((((f).insert + 1) % FIFO_SIZE) == (f).remove)

#define fifo_insert(f, c) do { \
	(f).fifo[(f).insert] = (c); \
	(f).insert = ((f).insert + 1) % FIFO_SIZE; \
} while (0)

#define fifo_remove(f, c) do { \
	(c) = (f).fifo[(f).remove];	   \
	(f).remove = ((f).remove + 1) % FIFO_SIZE; \
} while (0)

static volatile __xdata fifo_t rx_fifo;

void radio_txrx_isr(void) __interrupt RFTXRX_VECTOR
{
	uint8_t c = RFD;	// RX will overflow if we don't read the data
	if (!fifo_full(rx_fifo))
		fifo_insert(rx_fifo, c);
}

static volatile uint8_t rx_done;

void radio_rf_isr(void) __interrupt RF_VECTOR
{
	S1CON &= ~(S1CON_RFIF_1 | S1CON_RFIF_0);
	if (RFIF & RFIF_IM_DONE) {
		rx_done = 1;
		RFIF &= ~RFIF_IM_DONE;
	}
}

static int recv_byte(void) __critical
{
	uint8_t c;

	while (fifo_empty(rx_fifo)) {
		if (rx_done) {
			rx_done = 0;
			return -1;
		}
		await_interrupt();
	}
	fifo_remove(rx_fifo, c);
	return c;
}

size_t radio_receive(uint8_t *buf, size_t len)
{
	int n;

	// improve RX sensitivity, per TI datasheet
	TEST2 = RF_TEST2_RX_LOW_DATA_RATE_MAGIC;
	TEST1 = RF_TEST1_RX_LOW_DATA_RATE_MAGIC;
	RFST = RFST_SRX;
	for (n = 0; n < len; ++n) {
		int c = recv_byte();
		if (c == -1)
			break;
		buf[n] = c;
	}
	return n;
}

void radio_transmit(const uint8_t *buf, size_t len)
{
	int n;

	TEST2 = RF_TEST2_NORMAL_MAGIC;
	TEST1 = RF_TEST1_TX_MAGIC;
	RFST = RFST_STX;
	for (n = 0; n < len; ++n) {
		while (!RFTXRXIF)
			nop();
		RFTXRXIF = 0;
		RFD = buf[n];
	}
	RFST = RFST_SIDLE;
}
