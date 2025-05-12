#!/bin/bash

# Qemu AARCH64 emulator
QEMUBIN=qemu-system-aarch64

# board being emulated is virt
MACHINE=virt
CPU=cortex-a57

${QEMUBIN} -nographic -machine ${MACHINE} -cpu ${CPU} \
    -s -S \
    -kernel kernel.elf -nographic \
    -monitor telnet:127.0.0.1:55555,server,nowait
