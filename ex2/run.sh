#!/bin/bash

# Qemu AARCH64 emulator
QEMUBIN=qemu-system-aarch64

# board being emulated is vexpress-a9

# Hint: what cpu is on that board?  ARM cortex A9 ? Is it 32 or 64 bits?
MACHINE=vexpress-a9
CPU=cortex-a9

${QEMUBIN} -nographic -machine ${MACHINE} -cpu ${CPU} \
    -audio none \
    -kernel kernel.elf -nographic
