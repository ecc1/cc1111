#ifndef _LED_H
#define _LED_H

#include <stdint.h>

void led_init(void);

void led_toggle(void);

// Blink LED once per period_ms.
// Blink count times, or indefinitely if count == 0.

void blink(uint16_t period_ms, uint16_t count);

#endif /* _LED_H */
