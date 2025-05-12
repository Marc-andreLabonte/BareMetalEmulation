
// from: https://wiki.osdev.org/QEMU_AArch64_Virt_Bare_Bones

#include <stdint.h>

volatile uint8_t *uart = (uint8_t *) 0x09000000;

void putchar(char c) {
    *uart = c;
}

void print(const char *s) {
    while(*s != '\0') {
        putchar(*s);
        s++;
    }
}

void kmain(void) {
     print("Hello world!\n");
}
