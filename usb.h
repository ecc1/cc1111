#ifndef _USB_H
#define _USB_H

#include "cc1111.h"

void usb_init(void);

void use_usb_stdio(void);

int usb_pollc(void);

char usb_getc(void);

void usb_putc(char c);

void usb_flush(void);

void usb_enable(void);

void usb_disable(void);

void usb_isr(void) __interrupt P2INT_VECTOR;

#endif /* _USB_H */
