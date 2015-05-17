#include "clock.h"

#include "arch.h"

// Based on AltOS cc1111/ao_timer.c

volatile __data uint16_t tick_count;

void timer_isr(void) __interrupt T1_VECTOR
{
	++tick_count;
}

uint16_t time(void) __critical
{
	return tick_count;
}

void timer_init(void)
{
	// 24MHz (clock) / 8 (prescale divider) => 3MHz counter rate
	// 3MHz / 30000 => 100Hz tick rate
	const uint16_t modulus = 30000;

	// disable timer 1
	T1CTL = 0;

	// set the sample rate */
	T1CC0H = modulus >> 8;
	T1CC0L = modulus & 0xFF;

	T1CCTL0 = T1CCTL_MODE_COMPARE;
	T1CCTL1 = 0;
	T1CCTL2 = 0;

	// clear timer value
	T1CNTL = 0;

	// enable overflow interrupt
	OVFIM = 1;

	// enable timer 1 interrupt
	T1IE = 1;

	// enable timer 1 in modulo mode, dividing by 8
	T1CTL = T1CTL_MODE_MODULO | T1CTL_DIV_8;

	// global interrupt enable
	EA = 1;
}

void clock_init(void)
{
	const uint8_t speed_mask = CLKCON_TICKSPD_MASK | CLKCON_CLKSPD_MASK;
	const uint8_t speed_value = CLKCON_TICKSPD_1 | CLKCON_CLKSPD_1;

	// Power up both oscillators
	SLEEP &= ~SLEEP_OSC_PD;

	// Switch system clock to crystal oscilator
	CLKCON &= ~CLKCON_OSC_MASK;
	while (CLKCON & CLKCON_OSC_MASK)
		nop();

	CLKCON = (CLKCON & ~speed_mask) | speed_value;
	while ((CLKCON & speed_mask) != speed_value)
		nop();

	// Power down the unused RC oscillator
	SLEEP |= SLEEP_OSC_PD;
}
