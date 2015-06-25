// Transfer data between UART and USB

#include <stdio.h>
#include "arch.h"
#include "clock.h"
#include "serial.h"
#include "usb.h"

#define concat(x, y)	x ## y

#define DEFINE_POLLCHAR(M)			\
	int concat(M, _pollchar)(void)		\
	{					\
		int c = concat(M, _pollc)();	\
		if (c == '\r')			\
			return '\n';		\
		return c;			\
	}

#define DEFINE_PUTCHAR(M)			\
	void concat(M, _putchar)(char c)	\
	{					\
		if (c == '\n')			\
			concat(M, _putc)('\r');	\
		concat(M, _putc)(c);		\
	}

DEFINE_POLLCHAR(serial)
DEFINE_PUTCHAR(serial)

DEFINE_POLLCHAR(usb)
DEFINE_PUTCHAR(usb)

void main(void)
{
	clock_init();
	serial_init();
	usb_init();
	enable_interrupts();

	for (;;) {
		int c;
		if ((c = serial_pollchar()) != -1) {
			usb_putchar(c);
			usb_flush();
		}
		if ((c = usb_pollchar()) != -1) {
			serial_putchar(c);
		}
		run_background_tasks();
	}
}
