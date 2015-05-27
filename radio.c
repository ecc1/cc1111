#include "radio.h"

#include "arch.h"
#include "clock.h"
#include "rf.h"

uint8_t *packet_buf;
size_t packet_len;
size_t packet_pos;

volatile uint8_t packet_done;

#define buffer_init(buf, len) do { \
	packet_buf = (buf); \
	packet_len = (len); \
	packet_pos = 0; \
	packet_done = 0; \
} while (0)

#define end_of_buffer()	(packet_pos == packet_len)

enum { IDLE, RECEIVING, TRANSMITTING } radio_state;

void radio_txrx_isr(void) __interrupt RFTXRX_VECTOR
{
	uint8_t c;

	switch (radio_state) {
	case IDLE:
		return;
	case RECEIVING:
		c = RFD;
		if (packet_pos != packet_len && c != 0x00) {
			packet_buf[packet_pos++] = c;
			return;
		}
		break;
	case TRANSMITTING:
		if (packet_pos != packet_len) {
			RFD = packet_buf[packet_pos++];
			return;
		}
		break;
	}
	packet_done = 1;
	RFTXRXIE = 0;
}

size_t radio_receive(uint8_t *buf, size_t len, uint16_t timeout)
{
	uint16_t deadline = timeout ? time() + timeout : 0;

	buffer_init(buf, len);

	// improve RX sensitivity, per TI datasheet
	TEST2 = RF_TEST2_RX_LOW_DATA_RATE_MAGIC;
	TEST1 = RF_TEST1_RX_LOW_DATA_RATE_MAGIC;

	radio_state = RECEIVING;
	RFST = RFST_SRX;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_RX)
		nop();

	// Enable radio interrupts
	RFTXRXIE = 1;

	while (!packet_done && (!deadline || time() <= deadline))
		idle();

	radio_state = IDLE;
	RFST = RFST_SIDLE;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_IDLE)
		nop();

	return packet_pos;
}

void radio_transmit(uint8_t *buf, size_t len)
{
	// Pad end of packet with 2 null bytes
	buf[len++] = 0x00;
	buf[len++] = 0x00;

	buffer_init(buf, len);

	TEST2 = RF_TEST2_NORMAL_MAGIC;
	TEST1 = RF_TEST1_TX_MAGIC;

	radio_state = TRANSMITTING;
	RFST = RFST_STX;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_TX)
		nop();

	// Enable radio interrupts
	RFTXRXIE = 1;

	while (!packet_done)
		idle();

	radio_state = IDLE;
	RFST = RFST_SIDLE;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_IDLE)
		nop();
}

void radio_init(void)
{
	rf_init();
}
