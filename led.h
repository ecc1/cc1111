#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>

void led_init(void);

// Blink LED once per period_ms.
// Blink count times, or indefinitely if count == 0.

void blink(uint16_t period_ms, uint16_t count);

#endif // _LED_H_
