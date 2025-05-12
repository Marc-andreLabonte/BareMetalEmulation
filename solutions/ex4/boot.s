.global _start
_start:
    ldr r11, =stack_top
    mov sp, r11
    bl kmain
    b .
