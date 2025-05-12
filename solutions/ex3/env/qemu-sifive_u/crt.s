# See LICENSE for license details.

.section .text.init,"ax",@progbits
.globl _start

# FIXME: should set stack pointer first
_start:
    la      sp, __stack_top
    j       main
