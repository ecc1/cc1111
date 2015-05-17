#ifndef _CLOCK_H
#define _CLOCK_H

#include <stdint.h>
#include "cc1111.h"

#define HZ	100

void clock_init(void);

void timer_init(void);

// Current time, in number of ticks since initialization

uint16_t time(void);

void timer_isr(void) __interrupt T1_VECTOR;

#endif /* _CLOCK_H */
