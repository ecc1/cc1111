#include "radio.h"

#include "arch.h"
#include "rf.h"

void radio_init(void)
{
	rf_init();
}

#define FIFO_SIZE	1024

typedef struct {
	uint16_t insert;
	uint16_t remove;
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
	uint8_t c;

	c = RFD;
	if (c == 0x00)
		RFTXRXIE = 0;
	else if (!fifo_full(rx_fifo))
		fifo_insert(rx_fifo, c);
}

int radio_getc(void) __critical
{
	uint8_t c;

	while (fifo_empty(rx_fifo)) {
		if (!RFTXRXIE)
			return -1;
		await_interrupt();
	}
	fifo_remove(rx_fifo, c);
	return c;
}

void radio_receive(void)
{
	RFST = RFST_SIDLE;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_IDLE)
		nop();

	// improve RX sensitivity, per TI datasheet
	TEST2 = RF_TEST2_RX_LOW_DATA_RATE_MAGIC;
	TEST1 = RF_TEST1_RX_LOW_DATA_RATE_MAGIC;

	RFST = RFST_SRX;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_RX)
		nop();

	// Enable radio interrupts
	RFTXRXIF = 0;
	RFTXRXIE = 1;
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
