#!/bin/bash

#QEMUBIN=$HOME/src/qemu/build/qemu-system-riscv64

# Qemu RISCV64 emulator
QEMUBIN=qemu-system-riscv64

# board being emulated is sifive_u

MACHINE=sifive_u


${QEMUBIN} -nographic -machine ${MACHINE} \
    -audio none \
    -kernel build/bin/rv64imac/qemu-sifive_u/hello 
