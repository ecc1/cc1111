#include "cc1111.h"

void serial_init(void)
{
    // set up UART0 using Alternative Location 1
    PERCFG &= ~PERCFG_U0CFG_ALT_MASK;
    PERCFG |= PERCFG_U0CFG_ALT_1;

    U0CSR |= UxCSR_MODE_UART;

    // TX on P0_3, RX on P0_2
    P0DIR |= (1 << 3);
    P0DIR &= ~(1 << 2);

#if BAUD_RATE==9600
    U0BAUD = 163;
    U0GCR = 8;
#elif BAUD_RATE==57600
    U0BAUD = 59;
    U0GCR = 11;
#elif BAUD_RATE==115200
    U0BAUD = 59;
    U0GCR = 12;
#else
#error "BAUD_RATE is undefined"
#endif

    U0UCR = (UxUCR_FLUSH |
             UxUCR_FLOW_DISABLE |
             UxUCR_D9_EVEN_PARITY |
             UxUCR_BIT9_8_BITS |
             UxUCR_PARITY_DISABLE |
             UxUCR_SPB_1_STOP_BIT |
             UxUCR_STOP_HIGH |
             UxUCR_START_LOW);

    // select peripheral function rather than GPIO for TX on P0_3
    // (not required for RX on P0_2)
    P0SEL |= (1 << 3);
}

void serial_putc(char c)
{
    U0DBUF = c;
    while (!UTX0IF)
        __asm nop __endasm;
    UTX0IF = 0;
}
