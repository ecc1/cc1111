#include "rf.h"

#include "arch.h"

void rf_init(void)
{
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

	MCSM1 = RF_MCSM1_CCA_MODE_RSSI_BELOW_UNLESS_RECEIVING |
		RF_MCSM1_RXOFF_MODE_IDLE |
		RF_MCSM1_TXOFF_MODE_IDLE;	// (default)

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

	// use PA_TABLE 1 for transmitting '1' in ASK
	// (PA_TABLE 0 is always used for '0')
	FREND0 = (1 << RF_FREND0_LODIV_BUF_CURRENT_TX_SHIFT) |
		(1 << RF_FREND0_PA_POWER_SHIFT);

	FSCAL3 = (0b11 << 6) | (0b10 << 4) | 0b1001;
	FSCAL2 = (1 << 5) | 0b01010;	// VCO high
	FSCAL1 = 0x00;
	FSCAL0 = 0x1F;

	TEST2 = RF_TEST2_NORMAL_MAGIC;	// (default)
	TEST1 = RF_TEST1_TX_MAGIC;
	TEST0 = (0b10 << 2) | 1;	// disable VCO selection calibration

	// Power amplifier output settings
	// (see Table 72 on page 207 of the datasheet)
	PA_TABLE7 = 0x00;
	PA_TABLE6 = 0x00;
	PA_TABLE5 = 0x00;
	PA_TABLE4 = 0x00;
	PA_TABLE3 = 0x00;
	PA_TABLE2 = 0x00;
#if FREQUENCY == 915
	PA_TABLE1 = 0xC0;	// 10dBm, 36mA
#elif FREQUENCY == 868
	PA_TABLE1 = 0xC2;	// 10dBm, 36mA
#endif
	PA_TABLE0 = 0x00;
}
