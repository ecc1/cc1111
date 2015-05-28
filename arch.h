#ifndef _ARCH_H
#define _ARCH_H

#include "cc1111.h"

#define disable_interrupts()	EA = 0

#define enable_interrupts()	EA = 1

#define idle()			PCON = PCON_IDLE

#define nop()			__asm nop __endasm

// call idle() from a __critical section

#define await_interrupt() do { \
	enable_interrupts(); \
	idle();	\
	disable_interrupts(); \
} while (0)

// store a 16-bit value into a pair of registers defined as <name>{H,L}

#define set_word(name, val)	do { \
	name##H = ((uint16_t) (val)) >> 8;	\
	name##L = ((uint16_t) (val)) & 0xFF;	\
} while (0)

#endif /* _ARCH_H */
