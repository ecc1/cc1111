#ifndef _DELAY_H_
#define _DELAY_H_

#include <stdint.h>

#define nop()	__asm nop __endasm

void delay(uint16_t ms);

#endif  // _DELAY_H_
