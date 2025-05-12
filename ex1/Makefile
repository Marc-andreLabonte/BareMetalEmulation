


all:
	aarch64-linux-gnu-as boot.s -o boot.o
	aarch64-linux-gnu-gcc -ffreestanding -c kernel.c -o kernel.o
	aarch64-linux-gnu-ld -nostdlib -Tlinker.ld boot.o kernel.o -o kernel.elf

