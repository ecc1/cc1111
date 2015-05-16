#ifndef _DELAY_H
#define _DELAY_H

#include <stdint.h>

#define nop()	__asm nop __endasm

void delay(uint16_t ms);

#endif /* _DELAY_H */
