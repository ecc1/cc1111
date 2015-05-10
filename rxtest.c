// Poll for Medtronic packets and print them over the UART

#include <stdio.h>
#include "cc1111.h"
#include "clock.h"
#include "delay.h"
#include "radio.h"
#include "serial.h"

void main(void)
{
    static char __xdata packet[256];

    clock_init();
    radio_init();
    serial_init();

    while (1) {
        int length, i;

        length = radio_receive(packet, sizeof(packet));
        printf("\r\nReceived %d-byte packet:\r\n", length);
        for (i = 0; i < length; ++i)
            printf("%02X ", packet[i]);
        printf("\r\n");
        delay(1000);
    }
}

void putchar(char c) {
    serial_putc(c);
}
