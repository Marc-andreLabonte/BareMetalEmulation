#!/bin/bash

#QEMUBIN=$HOME/src/qemu/build/qemu-system-riscv64

# Qemu RISCV64 emulator
QEMUBIN=qemu-system-riscv64

# board being emulated is sifive_u

MACHINE=sifive_u

${QEMUBIN} -nographic -machine ${MACHINE} \
    -s -S \
    -kernel build/bin/rv64imac/qemu-sifive_u/hello \
    -monitor telnet:127.0.0.1:55555,server,nowait -nographic


#-device loader,file=hello,addr=0x80000000 \
