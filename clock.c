// Based on src/cc1111/ao_timer.c from git://git.gag.com/fw/altos
//
// Copyright © 2009 Keith Packard <keithp@keithp.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.

#include "clock.h"

#include "arch.h"

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
	set_word(T1CC0, modulus);

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
