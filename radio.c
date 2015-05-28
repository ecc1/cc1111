#include "radio.h"

#include "arch.h"
#include "clock.h"
#include "dma.h"
#include "rf.h"

uint8_t *packet_buf;
size_t packet_len;
size_t packet_pos;

volatile uint8_t rx_done;

void radio_txrx_isr(void) __interrupt RFTXRX_VECTOR
{
	uint8_t c;

	c = RFD;
	if (c == 0x00 || packet_pos == packet_len) {
		rx_done = 1;
	} else {
		packet_buf[packet_pos++] = c;
	}
}

volatile uint8_t tx_done;

void radio_rf_isr(void) __interrupt RF_VECTOR
{
	S1CON &= ~(S1CON_RFIF_1 | S1CON_RFIF_0);
	RFIF &= ~RFIF_IM_DONE;
	tx_done = 1;
}

size_t radio_receive(uint8_t *buf, size_t len, uint16_t timeout)
{
	uint16_t deadline = timeout ? time() + timeout : 0;

	packet_buf = buf;
	packet_len = len;
	packet_pos = 0;
	PKTLEN = 0xFF;

	// improve RX sensitivity, per TI datasheet
	TEST2 = RF_TEST2_RX_LOW_DATA_RATE_MAGIC;
	TEST1 = RF_TEST1_RX_LOW_DATA_RATE_MAGIC;

	rx_done = 0;
	RFST = RFST_SRX;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_RX)
		nop();

	while (!rx_done && (!deadline || time() <= deadline))
		idle();

	RFST = RFST_SIDLE;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_IDLE)
		nop();

	return packet_pos;
}

void radio_transmit(uint8_t *buf, size_t len)
{
	// terminate end of packet with null byte
	buf[len++] = 0x00;

	dma_set_transmit(buf, len);
	PKTLEN = len;

	dma_start();

	TEST2 = RF_TEST2_NORMAL_MAGIC;
	TEST1 = RF_TEST1_TX_MAGIC;

	tx_done = 0;
	RFST = RFST_STX;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_TX)
		nop();

	while (!tx_done)
		idle();

	RFST = RFST_SIDLE;
	while ((RF_MARCSTATE & RF_MARCSTATE_MASK) != RF_MARCSTATE_IDLE)
		nop();
}

void radio_init(void)
{
	rf_init();

	S1CON &= ~(S1CON_RFIF_1 | S1CON_RFIF_0);
	RFIF &= ~RFIF_IM_DONE;
	RFIM = RFIM_IM_DONE;
	IEN2 |= IEN2_RFIE;	// enable RF interrupt
	RFTXRXIE = 1;		// enable radio interrupt
}
