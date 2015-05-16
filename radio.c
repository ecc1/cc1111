#include "radio.h"

#include "cc1111.h"
#include "delay.h"

void radio_init(void)
{
    RFTXRXIE = 0;  // disable RF interrupts

    SYNC1 = 0xFF;
    SYNC0 = 0x00;  // sync word

    PKTLEN = 0xFF;  // packet length (default)

    PKTCTRL1 = 0x00;  // always accept sync word
                      // do not append status
                      // no address check

    PKTCTRL0 = 0x00;  // no whitening mode
                      // normal format
                      // disable CRC calculation and check
                      // fixed-length packet mode

    CHANNR = 0x00;  // channel number

    FSCTRL1 = 0x06;  // intermediate frequency (0x06 * 24MHz / 2^10 == 140.625kHz)
    FSCTRL0 = 0x00;  // frequency offset

    // 24-bit base frequency

#if FREQUENCY == 915

    // 0x263000 * 24Mhz / 2^16 == 916.5Mhz
    FREQ2 = 0x26; FREQ1 = 0x30; FREQ0 = 0x00;

#elif FREQUENCY == 868

    // 0x242E38 * 24Mhz / 2^16 == 868.333Mhz
    FREQ2 = 0x24; FREQ1 = 0x2E; FREQ0 = 0x38;

#else
#error "unknown FREQUENCY"
#endif

    // CHANBW_E = 1, CHANBW_M = 1, DRATE_E = 9
    // channel BW = 24MHz / (8 * (4 + CHANNBW_M) * 2^CHANBW_E) == 300kHz
    MDMCFG4 = (1 << 6) | (1 << 4) | 0x9;

    // data rate = (256 + DRATE_M) * 2^DRATE_E * 24MHz / 2^28 == 16.4kHz
    MDMCFG3 = 0x66;  // DRATE_M

    // enable DC blocking filter, ASK/OOK modulation,
    // disable Manchester encoding, 30/32 sync word bits
    MDMCFG2 = (0b011 << 4) | 0b011;

    // disable FEC, minimum preamble bytes = 16, CHANSPC_E = 2
    MDMCFG1 = (0b110 << 4) | 0b10;

    // channel spacing = (256 + CHANSPC_M) * 2^CHANSPC_E * 24MHz / 2^18 == 103.3KHz
    MDMCFG0 = 0x1A;  // CHANSPC_M

    MCSM2 = 0x07;  // (default)
    MCSM1 = 0b11 << 4;  // (default)

    // calibrate when going from IDLE, 0dB RX attenuation
    MCSM0 = (0b01 << 4) | (0b10 << 2) | 0b00;

    FOCCFG = (0b10 << 3) | (1 << 2) | 0b11;

    BSCFG = (1 << 6) + (0b10 << 4) + (1 << 3);

    AGCCTRL2 = 0b011;  // (default)
    AGCCTRL1 = 1 << 6;  // (default)
    AGCCTRL0 = (0b10 << 6) | (1 << 4) + 1;  // (default)

    FREND1 = (1 << 6) | (1 << 4) | (1 << 2) + 0b10;  // (default)

    // use PA_TABLE 2 for transmitting '1' in ASK (PA_TABLE 0 used for '0')
    FREND0 = (1 << 4) | 0x2;

    FSCAL3 = (0b11 << 6) | (0b10 << 4) | 0b1001;
    FSCAL2 = (1 << 5) | 0b01010;  // VCO high
    FSCAL1 = 0x00;
    FSCAL0 = 0x1F;

    TEST2 = 0x88;  // (default)
    TEST1 = 0x31;
    TEST0 = (0b10 << 2) + 1;  // disable VCO selection calibration stage

    // Power amplifier output settings
    PA_TABLE7 = 0x00; PA_TABLE6 = 0x00; PA_TABLE5 = 0x00; PA_TABLE4 = 0x00;
    PA_TABLE3 = 0x00; PA_TABLE2 = 0x52; PA_TABLE1 = 0x00; PA_TABLE0 = 0x00;
}

size_t radio_receive(uint8_t *buf, size_t len)
{
    int n;
    uint8_t prev_test1 = TEST1;

    TEST1 = 0x35;  // improve RX sensitivity, per TI datasheet
    RFST = RFST_SRX;
    for (n = 0; n < len; ++n) {
        while (!RFTXRXIF)
            nop();
        buf[n] = RFD;
        TCON &= ~TCON_RFTXRXIF;
        if (buf[n] == 0)
            break;
    }
    RFST = RFST_SIDLE;
    TEST1 = prev_test1;
    return n;
}

void radio_transmit(const uint8_t *buf, size_t len)
{
    int n;
    uint8_t prev_test1 = TEST1;

    TEST1 = 0x31;  // per TI datasheet
    RFST = RFST_STX;
    for (n = 0; n < len; ++n) {
        while (!RFTXRXIF)
            nop();
        TCON &= ~TCON_RFTXRXIF;
        RFD = buf[n];
    }
    RFST = RFST_SIDLE;
    TEST1 = prev_test1;
}
