#!/bin/bash

# Qemu AARCH64 emulator
# QEMUBIN=$HOME/src/qemu/build/qemu-system-aarch64
QEMUBIN=qemu-system-aarch64

# board being emulated is vexpress-a9
MACHINE=vexpress-a9
CPU=cortex-a9

${QEMUBIN}  \
    -machine ${MACHINE} -cpu ${CPU} \
    -s -S \
    -kernel kernel.elf -nographic \
    -device loader,file=rootfs.enc,addr=0x40000000,force-raw=on \
    -monitor telnet:127.0.0.1:55555,server,nowait

