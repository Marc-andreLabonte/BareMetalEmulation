#!/bin/bash

# Qemu AARCH64 emulator
QEMUBIN=qemu-system-aarch64

# board being emulated is virt
MACHINE=virt
CPU=cortex-a57


${QEMUBIN} -nographic -machine ${MACHINE} -cpu ${CPU} \
    -audio none \
    -kernel kernel.elf -nographic
