#include <stdio.h>

void main()
{
    const char *s = "Hello World!.\n";
    while (*s) putchar(*s++);
    while(1);
}
