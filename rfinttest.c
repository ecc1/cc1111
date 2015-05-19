#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "delay.h"
#include "radio.h"
#include "serial.h"

volatile uint16_t txrx_count;

void radio_txrx_isr(void) __interrupt RFTXRX_VECTOR
{
	uint8_t c = RFD;	// RX will overflow if we don't read the data
	++txrx_count;
}

volatile uint16_t rf_count;
volatile uint8_t rfif;

void radio_rf_isr(void) __interrupt RF_VECTOR
{
	++rf_count;
	rfif = RFIF;
	S1CON &= ~(S1CON_RFIF_1 | S1CON_RFIF_0);
	RFIF &= ~RFIF_IM_DONE;
}

void main(void)
{
	clock_init();
	radio_init();
	serial_init();

	RFTXRXIE = 1;

	RFIF = 0;
	RFIM = RFIM_IM_DONE;
	IEN2 |= IEN2_RFIE;

	enable_interrupts();

	// improve RX sensitivity, per TI datasheet
	TEST2 = RF_TEST2_RX_LOW_DATA_RATE_MAGIC;
	TEST1 = RF_TEST1_RX_LOW_DATA_RATE_MAGIC;

	while (1) {
		RFST = RFST_SRX;
		printf("TXRX: %d, RF: %d, RFIF: %02X, MARCSTATE: %02X\n",
		       txrx_count, rf_count, rfif, RF_MARCSTATE);
		delay(500);
	}
}
