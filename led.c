#include <stdint.h>
#include <cc1110.h>
#include "board.h"
#include "delay.h"

#define LED_DIR		dir_register(LED_PORT)
#define LED_BIT		port_bit(LED_PORT, LED_PIN)

void led_init(void)
{
    LED_DIR |= (1 << LED_PIN);  // set output for LED
}

void blink_once(uint16_t period_ms)
{
    uint16_t half = period_ms / 2;
    LED_BIT ^= 1;
    delay(half);
    LED_BIT ^= 1;
    delay(half);
}

void blink(uint16_t period_ms, uint16_t count)
{
    uint16_t n = count;
    while (n-- || count == 0)
        blink_once(period_ms);
}
