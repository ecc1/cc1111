#include "radio.h"

#include "arch.h"

void radio_init(void)
{
	RFTXRXIE = 0;		// disable RF interrupts

	SYNC1 = 0xFF;
	SYNC0 = 0x00;		// sync word

	PKTLEN = 0xFF;		// packet length (default)

	PKTCTRL1 = 0x00;	// always accept sync word
				// do not append status
				// no address check

	PKTCTRL0 = 0x00;	// no whitening mode
				// normal format
				// disable CRC calculation and check
				// fixed-length packet mode

	CHANNR = 0x00;		// channel number

	FSCTRL1 = 0x06;		// intermediate frequency
				// 0x06 * 24MHz / 2^10 == 140.625kHz

	FSCTRL0 = 0x00;		// frequency offset

	// 24-bit base frequency

#if FREQUENCY == 915

	// 0x263000 * 24Mhz / 2^16 == 916.5Mhz
	FREQ2 = 0x26;
	FREQ1 = 0x30;
	FREQ0 = 0x00;

#elif FREQUENCY == 868

	// 0x242E38 * 24Mhz / 2^16 == 868.333Mhz
	FREQ2 = 0x24;
	FREQ1 = 0x2E;
	FREQ0 = 0x38;

#else
#error "unknown FREQUENCY"
#endif

	// CHANBW_E = 1, CHANBW_M = 1, DRATE_E = 9
	// channel BW = 24MHz / (8 * (4 + CHANNBW_M) * 2^CHANBW_E) == 300kHz
	MDMCFG4 = (1 << RF_MDMCFG4_CHANBW_E_SHIFT) |
		(1 << RF_MDMCFG4_CHANBW_M_SHIFT) |
		(9 << RF_MDMCFG4_DRATE_E_SHIFT);

	// DRATE_M = 102
	// data rate = (256 + DRATE_M) * 2^DRATE_E * 24MHz / 2^28 == 16.4kHz
	MDMCFG3 = 0x66;

	MDMCFG2 = RF_MDMCFG2_DEM_DCFILT_ON |
		RF_MDMCFG2_MOD_FORMAT_ASK_OOK |
		RF_MDMCFG2_SYNC_MODE_30_32;

	// CHANSPC_E = 2
	MDMCFG1 = RF_MDMCFG1_FEC_DIS |
		RF_MDMCFG1_NUM_PREAMBLE_16 |
		(2 << RF_MDMCFG1_CHANSPC_E_SHIFT);

	// CHANSPC_M = 26
	// channel spacing
	// (256 + CHANSPC_M) * 2^CHANSPC_E * 24MHz / 2^18 == 103.3KHz
	MDMCFG0 = 0x1A;

	MCSM2 = RF_MCSM2_RX_TIME_END_OF_PACKET;	// (default)

	MCSM1 = RF_MCSM1_CCA_MODE_RSSI_BELOW_UNLESS_RECEIVING;	// (default)

	MCSM0 = RF_MCSM0_FS_AUTOCAL_FROM_IDLE |
		RF_MCSM0_MAGIC_3 |
		RF_MCSM0_CLOSE_IN_RX_0DB;

	FOCCFG = RF_FOCCFG_FOC_PRE_K_3K |
		RF_FOCCFG_FOC_POST_K_PRE_K_OVER_2 |
		RF_FOCCFG_FOC_LIMIT_BW_OVER_2;

	BSCFG = RF_BSCFG_BS_PRE_K_2K |
		RF_BSCFG_BS_PRE_KP_3KP |
		RF_BSCFG_BS_POST_KI_PRE_KI_OVER_2 |
		RF_BSCFG_BS_LIMIT_0;

	AGCCTRL2 = RF_AGCCTRL2_MAX_DVGA_GAIN_ALL |
		RF_AGCCTRL2_MAX_LNA_GAIN_0 |
		RF_AGCCTRL2_MAGN_TARGET_33dB;	// (default)

	AGCCTRL1 = RF_AGCCTRL1_AGC_LNA_PRIORITY_1 |
		RF_AGCCTRL1_CARRIER_SENSE_REL_THR_DISABLE |
		RF_AGCCTRL1_CARRIER_SENSE_ABS_THR_0DB;	// (default)

	AGCCTRL0 = RF_AGCCTRL0_HYST_LEVEL_MEDIUM |
		RF_AGCCTRL0_WAIT_TIME_16 |
		RF_AGCCTRL0_AGC_FREEZE_NORMAL |
		RF_AGCCTRL0_FILTER_LENGTH_16;	// (default)

	FREND1 = (1 << RF_FREND1_LNA_CURRENT_SHIFT) |
		(1 << RF_FREND1_LNA2MIX_CURRENT_SHIFT) |
		(1 << RF_FREND1_LODIV_BUF_CURRENT_RX_SHIFT) |
		(2 << RF_FREND1_MIX_CURRENT_SHIFT);	// (default)

	// use PA_TABLE 2 for transmitting '1' in ASK
	// (PA_TABLE 0 is always used for '0')
	FREND0 = (1 << RF_FREND0_LODIV_BUF_CURRENT_TX_SHIFT) |
		(2 << RF_FREND0_PA_POWER_SHIFT);

	FSCAL3 = (0b11 << 6) | (0b10 << 4) | 0b1001;
	FSCAL2 = (1 << 5) | 0b01010;	// VCO high
	FSCAL1 = 0x00;
	FSCAL0 = 0x1F;

	TEST2 = RF_TEST2_NORMAL_MAGIC;	// (default)
	TEST1 = RF_TEST1_TX_MAGIC;
	TEST0 = (0b10 << 2) | 1;	// disable VCO selection calibration

	// Power amplifier output settings
	PA_TABLE7 = 0x00;
	PA_TABLE6 = 0x00;
	PA_TABLE5 = 0x00;
	PA_TABLE4 = 0x00;
	PA_TABLE3 = 0x00;
	PA_TABLE2 = 0x52;
	PA_TABLE1 = 0x00;
	PA_TABLE0 = 0x00;

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
		TCON &= ~TCON_RFTXRXIF;
		RFD = buf[n];
	}
	RFST = RFST_SIDLE;
}
