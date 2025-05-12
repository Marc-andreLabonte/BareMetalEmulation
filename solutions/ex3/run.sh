#!/bin/bash

~/src/qemu/build/qemu-system-riscv64 \
    -bios none \
    -machine sifive_u \
    -device loader,file=hello,addr=0x80000000 \
    -monitor telnet:127.0.0.1:55555,server,nowait -nographic
