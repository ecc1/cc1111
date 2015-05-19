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

#endif /* _ARCH_H */
