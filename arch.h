#ifndef _ARCH_H
#define _ARCH_H

#include "cc1111.h"

#define nop()	__asm nop __endasm

#define await_interrupt() do { \
	EA = 1; \
	PCON = PCON_IDLE; \
	EA = 0; \
} while (0)

#endif /* _ARCH_H */
