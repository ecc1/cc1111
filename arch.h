#ifndef _ARCH_H
#define _ARCH_H

#include "cc1111.h"

void run_background_tasks(void);

#define disable_interrupts()	EA = 0

#define enable_interrupts()	EA = 1

#define idle()			run_background_tasks()

#define nop()			__asm nop __endasm

// call idle() from a __critical section

#define await_interrupt()			\
	do {					\
		enable_interrupts();		\
		idle();				\
		disable_interrupts();		\
	} while (0)

// store a 16-bit value into a pair of registers defined as <name>{H,L}

#define set_word(name, val)				\
	do {						\
		name##H = ((uint16_t) (val)) >> 8;	\
		name##L = ((uint16_t) (val)) & 0xFF;	\
	} while (0)

// list bytes in little-endian order for use in initializers

#define LE_WORD(x)    ((x) & 0xFF), ((uint8_t) (((uint16_t) (x)) >> 8))

#endif /* _ARCH_H */
