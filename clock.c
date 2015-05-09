#include "cc1111.h"
#include "delay.h"

// Taken from AltOS cc1111/ao_timer.c

void clock_init(void)
{
    /* Power up both oscillators */
    SLEEP &= ~SLEEP_OSC_PD;

    /* Switch to the HFRC oscillator */
    CLKCON = (CLKCON & ~CLKCON_OSC_MASK) | (CLKCON_OSC_RC);

    /* Wait for the HFRC oscillator to be stable */
    while (!(SLEEP & SLEEP_HFRC_STB))
        nop();

    /* Delay for 'a while' waiting for the crystal to
     * stabilize -- the XOSC_STB bit isn't reliable
     *
     *  http://www.ti.com/lit/er/swrz022c/swrz022c.pdf
     */
    delay(100);

    /* Switch system clock to crystal oscilator */
    CLKCON = (CLKCON & ~CLKCON_OSC_MASK) | (CLKCON_OSC_XTAL);

    /* Wait for the HFRC oscillator to be stable */
    while (!(SLEEP & SLEEP_XOSC_STB))
        nop();

    /* Power down the unused HFRC oscillator */
    SLEEP |= SLEEP_OSC_PD;

    /* Crank up the timer tick and system clock speed */
    CLKCON = ((CLKCON & ~(CLKCON_TICKSPD_MASK | CLKCON_CLKSPD_MASK)) |
              (CLKCON_TICKSPD_1 | CLKCON_CLKSPD_1));

    while ((CLKCON & (CLKCON_TICKSPD_MASK|CLKCON_CLKSPD_MASK)) !=
           (CLKCON_TICKSPD_1 | CLKCON_CLKSPD_1))
        nop();
}
