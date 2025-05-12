// See LICENSE for license details.

#include <stdio.h>

enum {
    /* UART Registers */
    UART_REG_TXFIFO = 0,
};

/* from sifive_u.dts 10010000
    chosen {
        stdout-path = "/soc/serial@10010000";
    };

    aliases {
        serial0 = "/soc/serial@10010000";
        serial1 = "/soc/serial@10011000";
        ethernet0 = "/soc/ethernet@10090000";
    };
*/
static volatile int *uart = (int *)(void *)0x10010000;

int putchar(int ch)
{
    while (uart[UART_REG_TXFIFO] < 0);
    return uart[UART_REG_TXFIFO] = ch & 0xff;
}
