


all:
	arm-none-eabi-as boot.s -o boot.o
	arm-none-eabi-gcc -ffreestanding -c kernel.c -o kernel.o
	arm-none-eabi-ld -nostdlib -Tlinker.ld boot.o kernel.o -o kernel.elf

