#include "led.h"

#include "cc1111.h"
#include "board.h"
#include "delay.h"

void led_init(void)
{
#if BOARD == BOARD_TI_DONGLE

#define LED_BIT		P1_1

	P1DIR |= (1 << 1);
	P1_1 = 0;

#elif BOARD == BOARD_SRF_STICK

#define LED_BIT		P1_7

	P1DIR |= (1 << 7) | (1 << 6) | (1 << 5);
	P1_7 = 0;
	P1_6 = 0;
	P1_5 = 0;

#else
#error "unknown BOARD"
#endif
}

void led_toggle(void)
{
	LED_BIT ^= 1;
}

static void blink_once(uint16_t period_ms)
{
	uint16_t half = period_ms / 2;

	led_toggle();
	delay(half);

	led_toggle();
	delay(half);
}

void blink(uint16_t period_ms, uint16_t count)
{
	uint16_t n = count;
	while (n-- || count == 0)
		blink_once(period_ms);
}
